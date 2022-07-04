
#define TRUE 1
#define FALSE 0

#define HIGH 1
#define LOW 0

enum PLAYBACK_MODE
{
    START,
    ALPHA_TEAM_WIN_EXPLOSION,
    BRAVO_TEAM_WIN_EXPLOSION,
    BRAVO_TEAM_WIN,
    BOMB_MOUNTED_SIREN,
    TIKTOK,
    GROUNDED_SIREN,

} playback;

volatile int counting_down_time = 0;
volatile int mounted = FALSE;
volatile int grounded = FALSE;
volatile int isPlaying = FALSE;
volatile int nowPlayingpPiority = 0;

int play_mp3(int mode)
{
    //TODO:
    return 0;   //successed
}

int SENSOR_INPUT()
{
    //TODO:
    return 0;   //return sensor status
}

int RFID_INPUT()
{
    //TODO:
    return 0;   //return rfid status
}

int hint_playback(int mode, int priority)
{
    if(isPlaying == FALSE || nowPlayingpPiority > priority) //not playing or lower priority
    {
        play_mp3(mode);
        nowPlayingpPiority = priority;
    }
    return 0;
}

void SysTick_Handler(void)  //1ms
{
    counting_down_time--;

    if(SENSOR_INPUT() == HIGH)
        grounded = TRUE;
    else
        grounded = FALSE;

    if(RFID_INPUT() == HIGH)
        mounted = TRUE;
    else
        mounted = FALSE;

    return;
}


int main(void)
{
    counting_down_time = keypad_input()*1000;    //msecond
    hint_playback(START, 0);
    while(counting_down_time > 0 && mounted == FALSE)
    {
        if(grounded == TRUE)
            hint_playback(GROUNDED_SIREN, 4);
        else
            hint_playback(TIKTOK, 5);
    }

    if(counting_down_time <= 0)
    {
        hint_playback(BRAVO_TEAM_WIN_EXPLOSION, 0);
        while(isPlaying == TRUE);
        return 0;
    }
    //else  //mounted

    //counting_down_time = 30*1000;  //msecond

    while(counting_down_time > 0 && mounted == TRUE)
    {
        hint_playback(BOMB_MOUNTED_SIREN, 5);
    }

    if(counting_down_time <= 0) //booooooom!!!!
    {
        hint_playback(ALPHA_TEAM_WIN_EXPLOSION, 0);
        while(isPlaying == TRUE);
        return 0;        
    }
    else    //bravo team unmounted
    {
        hint_playback(BRAVO_TEAM_WIN, 0);
        while(isPlaying == TRUE);
        return 0;
    }
}