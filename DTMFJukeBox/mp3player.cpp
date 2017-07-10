#include <arduino.h>
#include "mp3player.h"

MP3Player::MP3Player(Stream &comms)
{
  _comms = &comms;
}

MP3Player::~MP3Player()
{
}

void MP3Player::SetCommand(eCommands command, uint16_t data)
{
  CmdBlock.cmd = command;
  CmdBlock.data = data;
  SendToDevice((byte*)&CmdBlock);
}
void MP3Player::begin(bool feedback)
{
  parseState = WAIT_HDR;
  CmdBlock.hdr = 0x7e;
  CmdBlock.ver = 0xff;
  CmdBlock.length = 6;
  CmdBlock.feedback = feedback ? 1 : 0;
  CmdBlock.trl = 0xef;
  SetCommand(SELECT_DEV,0x0200);
  SetVolume(MAX_VOLUME/2);
}

void MP3Player::begin()
{
  begin(false);
}

void MP3Player::PlayFolderName(int folder,int file)
{
  currentFolder = folder;
  currentFile = file;
  SetCommand(PLAY_FOLDER_NAME,(file * 256)+folder);
}

void MP3Player::SetVolume(int value)
{
  currentVolume = value;
  SetCommand(SET_VOL,currentVolume<<8);
}

void MP3Player::VolumeUp()
{
  SetVolume(currentVolume+1); 
}
void  MP3Player::VolumeDown()
{
  SetVolume(currentVolume-1); 
}

void MP3Player::PlayNextInFolder()
{
  PlayFolderName(currentFolder,currentFile+1);
}

void MP3Player::Pause()
{
  SetCommand(PAUSE_PLAY,0);
}

void MP3Player::Resume()
{
  SetCommand(RESUME_PLAY,0);
}

void MP3Player::Next()
{
  SetCommand(NEXT_SONG,0);
}
void MP3Player::Previous()
{
  SetCommand(PREV_SONG,0);
}

void MP3Player::Feedback(byte b)
{
  switch (parseState)
  {
    case WAIT_HDR:
      if (b == 0x7e)
        parseState = WAIT_VER;
      break;
    case WAIT_VER:
        parseState = (b == 0xff) ? WAIT_LENGTH : WAIT_HDR;
        break;
    case WAIT_LENGTH:
        if (b > 0 && b <= 10)  // size of payload buffer
        {
          parseState = GET_DATA;
          feedbackLength = b;
          feedbackIndex = 0;
        }
        else
          parseState = WAIT_HDR;
        break;
    case GET_DATA:
      feebackPayload[feedbackIndex++] = b;
      if (feedbackIndex == feedbackLength)
      {
         parseState = WAIT_HDR;
         analyseFeedback();
      }
      break;
  }
}

void  MP3Player::analyseFeedback()
{
#if 1
  FeedbackCallback(feebackPayload[0],feebackPayload[3]);
#else
  switch ((enum eCommands)feebackPayload[0])
  {
    case FEEDBACK_REPLY:
      break;    // do nothing
    case FEEDBACK_ERROR:
      FeedbackCallback(FEEDBACK_ERROR,feebackPayload[3]);
      break;
    case TTRACK_FINISHED:
      FeedbackCallback(TTRACK_FINISHED,feebackPayload[3]);
      break;
  }
#endif
}

void MP3Player::FoldersPerDiskQuery()
{
  SetCommand(FOLDERS_PER_DISK,0);
}

void MP3Player::TracksPerFolderQuery(int folder)
{
  SetCommand(TRACKS_PER_FOLDER,folder<<8);  
}

void MP3Player::SendToDevice(byte data[])
{
  for(int i=0;i<8;i++)
    _comms->write(data[i]);
}

void MP3Player::FeedRead()
{
    while (_comms->available())
      Feedback(_comms->read());
}
