#include "Wave.h"

WaveIn::WaveIn( )
{
	hWaveIn = NULL;
}

WaveIn::~WaveIn( )
{
	close( );
}

void WaveIn::close( )
{
	waveInReset( hWaveIn );

	while ( !waveHeaders.empty( ) )
	{
		LPWAVEHDR pwh = ( LPWAVEHDR )waveHeaders.front( );

		waveInUnprepareHeader( hWaveIn, pwh, sizeof( WAVEHDR ) );

		delete[ ]pwh->lpData;
		delete waveHeaders.front( );
	}

	waveInClose( hWaveIn );
	hWaveIn = NULL;
}

BOOL WaveIn::open( int channels, int freq, int bits, HWND hWnd )
{
	wfex.cbSize = sizeof( wfex );
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = channels;
	wfex.nSamplesPerSec = freq;
	wfex.wBitsPerSample = bits;
	wfex.nBlockAlign = wfex.nChannels * ( wfex.wBitsPerSample / 8 );
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

	MMRESULT res;
	res = waveInOpen( &hWaveIn, WAVE_MAPPER, &wfex, ( DWORD )hWnd, 0, CALLBACK_WINDOW );
	return ( res == MMSYSERR_NOERROR );
}

BOOL WaveIn::addBuffer( size_t size )
{
	WAVEHDR* pwh = new WAVEHDR;
	pwh->dwFlags = 0;
	pwh->dwBufferLength = size;
	pwh->lpData = new char[ size ];

	waveHeaders.push_back( pwh );

	if ( waveInPrepareHeader( hWaveIn, pwh, sizeof( WAVEHDR ) ) ||
		 waveInAddBuffer( hWaveIn, pwh, sizeof( WAVEHDR ) ) )
	{
		close( );
		return FALSE;
	}
	return TRUE;
}

BOOL WaveIn::start( )
{
	return ( waveInStart( hWaveIn ) == MMSYSERR_NOERROR );
}


WaveOut::WaveOut( )
{
	hWaveOut = NULL;
}

WaveOut::~WaveOut( )
{
	close( );
}

void WaveOut::close( )
{
	waveOutReset( hWaveOut );
	waveOutClose( hWaveOut );
	hWaveOut = NULL;
}

BOOL WaveOut::open( int channels, int freq, int bits, HWND hWnd, int deviceID )
{
	wfex.cbSize = sizeof( wfex );
	wfex.wFormatTag = WAVE_FORMAT_PCM;
	wfex.nChannels = channels;
	wfex.nSamplesPerSec = freq;
	wfex.wBitsPerSample = bits;
	wfex.nBlockAlign = wfex.nChannels * ( wfex.wBitsPerSample / 8 );
	wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;

	MMRESULT res;
	res = waveOutOpen( &hWaveOut, deviceID, &wfex, ( DWORD )hWnd, 0, CALLBACK_WINDOW );
	return ( res == MMSYSERR_NOERROR );
}

void WaveOut::play( const char* buf, size_t size )
{
	WAVEHDR* pwh = new WAVEHDR;
	pwh->dwFlags = 0;
	pwh->dwLoops = 0;
	pwh->dwBufferLength = size;
	pwh->lpData = new char[ size ];
	memcpy( pwh->lpData, buf, size );

	waveOutPrepareHeader( hWaveOut, pwh, sizeof( WAVEHDR ) );
	waveOutWrite( hWaveOut, pwh, sizeof( WAVEHDR ) );
}