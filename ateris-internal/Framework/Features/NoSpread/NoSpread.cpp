#include "NoSpread.h"
#include "../../../Globals.h"

/*void CNoSpread::RunProjectile( CUserCmd* Cmd )
{
    if ( !gLocalPlayer || !gLocalWeapon || ( !TF2->IsProjectileWeapon( gLocalWeapon ) ) )
        return;

    TF2->SharedRandomInt(MD5_PseudoRandom(Cmd->command_number) & INT_MAX, "SelectWeightedSequence", 0, 0, 0 );

    for ( int i = 0; i < 6; ++i )
    {
        gRandomStream->RandomFloat( -1, 1 );
    }

    if ( gLocalWeapon->ItemDefinitionIndex( ) == Soldier_m_TheBeggarsBazooka )
    {
        if ( Cmd->buttons & IN_ATTACK )
            return;
    }

    else if ( gLocalWeapon->WeaponID( ) == TF_WEAPON_COMPOUND_BOW || gLocalWeapon->ItemDefinitionIndex( ) == Sniper_m_TheHuntsman || gLocalWeapon->ItemDefinitionIndex( ) == Sniper_m_FestiveHuntsman )
    {
        if ( Cmd->buttons & IN_ATTACK || gLocalWeapon->ChargeBeginTime( ) == 0.f )
        {
            return;
        }
    }

    else if ( !( Cmd->buttons & IN_ATTACK ) )
    {
        return;
    }

    if ( gLocalWeapon->WeaponID( ) == TF_WEAPON_SYRINGEGUN_MEDIC )
    {
        if (TF2->IsAttacking(Cmd))
        {
            Cmd->viewangles.x += TF2->RandomFloat(-1.5f, 1.5f);
            Cmd->viewangles.y += TF2->RandomFloat(-1.5f, 1.5f);
            gCvar->ConsolePrintf("Rolled: %f\n", TF2->RandomFloat(-1.5f, 1.5f));
        }
    }
    else
    {
        const Vector& view = gEngineClient->GetViewAngles( );
        Vector spread = gLocalWeapon->GetSpreadAngles( );

        if ( !spread.IsZero( ) )
        {
            spread -= view;
            Cmd->viewangles -= spread;
        }
    }
}*/

float CNoSpread::CalcMantissaStep( float Value )
{
    //convert the float's 4 bytes into an int, keeping the value of them
    int RawVal = reinterpret_cast< int& >( Value );

    //remove the mantissa from our new int, so we're left only with the exponent and sign bit, 8 + 1 = 9 bits
    int Exponent = ( RawVal >> 23 ) & 0xFF;

    //(2^8-1)-1 "???" + 23 "mantissa size"
    //could possibly be so we remove the first sign bit, might fuck with the integer's value
    return powf( 2, Exponent - ( 127 + 23 ) );
}

bool CNoSpread::IsPerfectShot( )
{
    auto ServerTime = gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick;
    auto TimeSinceAttack = ServerTime - gLocalWeapon->LastFireTime( );

    auto BulletsPerShot = gLocalWeapon->GetWeaponData( ).BulletsPerShot;

    if ( BulletsPerShot >= 1 )
        BulletsPerShot = TF2->AttribHookFloat( BulletsPerShot, "mult_bullets_per_shot", gLocalWeapon, 0x0, true );

    else BulletsPerShot = 1;

    if ( ( BulletsPerShot == 1 && TimeSinceAttack > 1.25 ) || ( BulletsPerShot > 1 && TimeSinceAttack > 0.25 ) )
        return true;

    return false;
}

bool CNoSpread::SendNetMessageHandler( INetMessage* data )
{
    // if we send clc_move with playerperf command or corrected angles, we must ensure it will be sent via reliable stream
    // otherwise you will get prediction errors left and right
    if ( ShouldUpdate )
    {
        if ( data->GetType( ) != clc_Move )
            return false;

        // and wait for post call
        IsWaitingForPostSNM = true;

        // Force reliable
        return true;
    }

    return false;
}

//if first handler ran successfully, run post original so that we send the server a request for our needed playerperf data
//the playerperf data contains the actual server uptime
void CNoSpread::SendNetMessagePostHandler( )
{
    static float time = gEngineClient->Time( );
    float elapsed = gEngineClient->Time( );

    if ( !IsWaitingForPostSNM || ( IsWaitingForPerfData && elapsed - time < 1.f ) || G->Doubletap.ToSend > 0 || G->Doubletap.Warp )
        return;

    IsWaitingForPostSNM = false;

    //this is the way that worked for me, sending a net_stringcmd did not work properly
    //if this doesnt work, try changing cl_debug_player_perf to 1
    gEngineClient->ClientCmd_Unrestricted( "playerperf" );

    ShouldUpdate = false;

    if ( SyncState == NOT_SYNCED )
        SentClientFloatTime = TF2->PlatFloatTime( );

    IsWaitingForPerfData = true;
    time = gEngineClient->Time( );
}

//now you need to receive the playerperf data
bool CNoSpread::DispatchUserMessageHandler( int type, bf_read* buf )
{
    bool should_call_original = true;

    if ( ( !IsWaitingForPerfData && !WasLastPerf ) || G->Doubletap.ToSend > 0 || G->Doubletap.Warp )
        return should_call_original;

    //filter out messages we dont need
    if ( type != 5 )
        return should_call_original;

    int message_dest = buf->ReadByte( );

    if ( message_dest != 2 )
    {
        buf->Seek( 0 );
        return should_call_original;
    }

    //we will need this for fixing correction on ping higher than local server
    double start_time = TF2->PlatFloatTime( );

    //start collecting the required info from the playerperf message
    //you only need the first float of the multiple the server sends you, since that is the most recent value of the server's time
    char msg_str[ 256 ];
    buf->ReadString( msg_str, sizeof( msg_str ) );
    buf->Seek( 0 );

    char server_time[ 32 ] = "";
    for ( int i = 0; i < sizeof( server_time ); i++ )
    {
        if ( msg_str[ i ] != ' ' )
            server_time[ i ] = msg_str[ i ];
        else
            break;
    }
    char* tmp;

    //convert the time from the string we got to a double
    double data = std::strtod( server_time, &tmp );
    ServerTime = data;

    //calculate the mantissa step required based off of the server's seed
    MantissaStep = CalcMantissaStep( data * 1000.0 );

    //server uptime is too low, you can just stop syncing here or set your timer to resync again after a certain period of time
    if ( MantissaStep < 1.0 )
    {
        IsWaitingForPerfData = false;
        WasLastPerf = true;
        should_call_original = false;
        IsBadMantissa = true;
        SyncState = NOT_SYNCED;
        return should_call_original;
    }

    //mantissa is good for correction and we have received a playerperf, no need to send another one
    IsBadMantissa = false;
    WasLastPerf = true;
    should_call_original = false;

    if ( !IsWaitingForPerfData )
        return should_call_original;

    //start correction process by getting ping difference between client and server
    if ( SyncState == NOT_SYNCED )
    {
        double client_time = TF2->PlatFloatTime( );

        //in theory we could just do m_sent_client_floattime - start_time, and we should get the same result
        double total_latency = ( client_time - ( client_time - start_time ) ) - SentClientFloatTime;
        FloatTimeDelta = data - SentClientFloatTime;
        FloatTimeDelta -= ( total_latency / 2.0 );
        IsWaitingForPerfData = false;
        SyncState = CORRECTING;
        IsSyncing = true;
    }

    else if ( SyncState != SYNCED )
    {
        //engine time difference between us and the server
        double time_difference = SentClientFloatTime - data;

        double mantissa_step = CalcMantissaStep( SentClientFloatTime * 1000.0 );
        //double correction_threshhold = config->m_seed_pred.m_extreme_prediction ? 0.001 : ( mantissa_step / 1000.0 / 2.0 );
        double correction_threshhold = mantissa_step / 1000.0 / 2.0;

        if ( abs( time_difference ) > correction_threshhold || abs( LastTimeCorrection ) > mantissa_step / 1000.0 )
        {
            FloatTimeDelta -= time_difference;
            IsResyncNeeded = true;
        }

        else  IsResyncNeeded = false;

        LastTimeCorrection = time_difference;
        IsWaitingForPerfData = false;

        //we are synced! time to get our predicted time to get the required seed for spread correction
        if ( !IsResyncNeeded )
            SyncState = SYNCED;
    }

    else {
        IsResyncNeeded = false;
        IsWaitingForPerfData = false;
    }

    return should_call_original;
};

//we do this here because cl_sendmove is the function that calls send_net_msg
void CNoSpread::CL_SendMoveHandler( )
{
    //return if we are not synced (couldnt get the right server time)
    if ( !SyncState || !Config->Misc->NoSpread->Enabled || G->Doubletap.ToSend > 0 || G->Doubletap.Warp )
        return;

    CurrentWeaponSpread = 0.0;

    if ( !gLocalPlayer || !gLocalWeapon || !gLocalWeapon->IsSpreadWeapon( ) )
        return;

    int new_packets = 1 + gClientState->chokedcommands;

    double asumed_real_time = TF2->PlatFloatTime( ) + FloatTimeDelta;
    double predicted_time = asumed_real_time;

    predicted_time += WriteUserCmdCorrection * new_packets;
    double ping = gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING );

    static float time = gEngineClient->Time( );
    float elapsed = gEngineClient->Time( );
    if ( elapsed - time > 1.f && gLocalPlayer->IsAlive( ) && !IsWaitingForPerfData )
    {
        time = gEngineClient->Time( );

        SentClientFloatTime = asumed_real_time;
        ShouldUpdate = true;

        if ( !gLocalPlayer->IsAlive( ) && SyncState != CORRECTING )
        {
            LastSyncDeltaTime = FloatTimeDelta;
            SyncState = DEAD_SYNC;
        }
    }

    // If we're dead just return original
    if ( !gLocalPlayer->IsAlive( ) )
        return;
    else {

        // We should cancel the dead sync process
        if ( SyncState == DEAD_SYNC )
        {
            // Restore delta time
            FloatTimeDelta = LastSyncDeltaTime;
            // Mark as synced
            SyncState = SYNCED;
            ShouldUpdate = false;
        }
    }

    // Bad weapon
    if ( gLocalWeapon && gLocalWeapon->Slot( ) >= 2 || !TF2->IsHitscanWeapon( gLocalWeapon ) )
        return;

    // Check if we are attacking, if not then no point in adjusting
    if ( !WasInAttack )
        return;

    if ( gLocalWeapon && IsPerfectShot( ) )
        return;

    if ( gLocalWeapon )
    {
        CurrentWeaponSpread = gLocalWeapon->GetWeaponSpread( );
    }

    if ( !std::isfinite( CurrentWeaponSpread ) )
    {
        return;
    }

    //We have our prediction seed, now we can use it to correct the spread by calling applycorrection in createmove
    PredictionSeed = ( float )( predicted_time * 1000.0f );

    //store in global for cl_sendmove_handler_post
    StartTime = TF2->PlatFloatTime( );
    NewPackets = new_packets;

    //here it calls cl_sendmove original
    //rest of the code is continued in another function, which runs after calling cl_sendmove
}

void CNoSpread::CL_SendMovePostHandler( )
{
    if ( !Config->Misc->NoSpread->Enabled || G->Doubletap.ToSend > 0 || G->Doubletap.Warp )
        return;

    if ( !gLocalPlayer || !gLocalWeapon || !gLocalWeapon->IsSpreadWeapon( ) )
        return;

    double time_end = TF2->PlatFloatTime( );

    // Update the processing time if we actually processed stuff
    if ( ShouldUpdateCmdCorrection )
    {
        // How long it took for us to process each cmd? We will add this number next time we will process usercommands
        WriteUserCmdCorrection = ( time_end - StartTime ) / NewPackets;
        ShouldUpdateCmdCorrection = false;
    }
}

void CNoSpread::CreateMoveHandler( CUserCmd* Cmd )
{
    if ( G->Doubletap.ToSend > 0 || G->Doubletap.Warp )
        return;

    if ( SyncState == SYNCED )
        IsSyncing = false;

    // Not synced currently, try to sync
    static float time = gEngineClient->Time( );
    float elapsed = gEngineClient->Time( );
    if ( SyncState == NOT_SYNCED && !IsBadMantissa )
    {
        IsSyncing = true;
        ShouldUpdate = true;
        time = gEngineClient->Time( );
        //i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "[CreateMove] Not synced currently, try to sync\n" );
    }
    // Else if mantissa bad, update every 10 mins
    else if ( SyncState == NOT_SYNCED && elapsed - time > 1.f )
        SyncState = CORRECTING;

    if ( ( SyncState != SYNCED && !IsResyncNeeded ) || CurrentWeaponSpread == 0.0 || !Config->Misc->NoSpread->Enabled )
        return;

    if ( !( Cmd->buttons & IN_ATTACK ) )
    {
        return;
    }

    ApplySpreadCorrection( Cmd->viewangles, reinterpret_cast< int& >( PredictionSeed ) & 0xFF, CurrentWeaponSpread );
    ShouldUpdateCmdCorrection = true;
}

void CNoSpread::ApplySpreadCorrection( Vector& angles, int seed, float spread )
{
    if ( !gLocalPlayer || !gLocalWeapon || G->Doubletap.ToSend > 0 || G->Doubletap.Warp )
        return;

    if ( !gLocalPlayer->IsAlive( ) )
        return;

    bool is_first_shot_perfect = IsPerfectShot( );

    int bullets_per_shot = gLocalWeapon->GetWeaponData( ).BulletsPerShot;

    if ( bullets_per_shot >= 1 )
        bullets_per_shot = TF2->AttribHookFloat( bullets_per_shot, "mult_bullets_per_shot", gLocalWeapon, 0x0, true );

    else bullets_per_shot = 1;

    if ( G->Aimbot.SavedWeaponID == TF_WEAPON_MINIGUN )
        bullets_per_shot = 4;

    if ( bullets_per_shot == 1 && is_first_shot_perfect )
        return;

    std::vector< Vector > bullet_corrections;
    Vector average_spread( 0.0f, 0.0f, 0.0f );

    RandomFloatVals.clear( );
    for ( int i = 0; i < bullets_per_shot; i++ )
    {
        TF2->RandomSeed( seed + i );
        auto x = TF2->RandomFloat( -0.5, 0.5 ) + TF2->RandomFloat( -0.5, 0.5 );
        auto y = TF2->RandomFloat( -0.5, 0.5 ) + TF2->RandomFloat( -0.5, 0.5 );

        if ( is_first_shot_perfect && !i )
        {
            x = 0.0f;
            y = 0.0f;
        }

        RandomFloatVals.push_back( { x, y } );

        gMath->ClampAngles( angles );
        auto forward = Vector( ), right = Vector( ), up = Vector( );
        gMath->AngleVectors( angles, &forward, &right, &up );

        Vector fixed_spread = forward + ( right * x * spread ) + ( up * y * spread );
        fixed_spread.NormalizeInPlace( );
        average_spread += fixed_spread;
        bullet_corrections.push_back( fixed_spread );
    }

    average_spread /= bullets_per_shot;
    Vector fixed_spread( FLT_MAX, FLT_MAX, FLT_MAX ); // i hate warnings

    for ( auto& spread : bullet_corrections )
    {
        if ( spread.DistTo( average_spread ) < fixed_spread.DistTo( average_spread ) )
            fixed_spread = spread;
    }

    Vector fixed_angles;
    gMath->VectorAngles( fixed_spread, fixed_angles );
    Vector correction = ( angles - fixed_angles );
    angles += correction;
    gMath->ClampAngles( angles );
}

void CNoSpread::Reset( )
{
    SyncState = NOT_SYNCED;
    IsWaitingForPerfData = false;
    ShouldUpdate = false;
    IsWaitingForPostSNM = false;
    IsResyncNeeded = false;
    IsSyncing = false;
    WasLastPerf = false;
    ShouldUpdateCmdCorrection = false;
    IsBadMantissa = false;
    WasInAttack = false;
    FloatTimeDelta = 0.0;
    SentClientFloatTime = 0.0;
    LastTimeCorrection = 0.0;
    WriteUserCmdCorrection = 0.0;
    LastSyncDeltaTime = 0.0;
    PredictionSeed = 0.0f;
    StartTime = 0.0f;
    MantissaStep = 0.0f;
    ServerTime = 0.0f;
    CurrentWeaponSpread = 0.0f;
    NewPackets = 0;
}