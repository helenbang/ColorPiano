// ColorPiano
// This is the Processing code for the project.
========================================================
import javax.sound.midi.*;
import processing.serial.*;
  Serial port;
  
private Synthesizer synthesizer;
private MidiChannel channel;      // channel we play on -- 10 is for percussion
private Instrument[] soundbankInstruments, synthesizerInstruments;
private int velocity = 64;        // default volume is 50%
private int midiNote = -1;
private int prevNote = -1;

private int program = 25;
  
String currentInstrument = "nothing selected yet";
PFont font;

float x,y;

int n = 100;
float [] xdata = new float[n];

void setup() {
  size (600, 600);
  background (127);
  
  println(Serial.list());
  port = new Serial(this, Serial.list()[4], 9600);
  port.bufferUntil('\n');  // don't generate a serialEvent() unless you get a  
                              newline character:
  
  Soundbank soundbank = null;
  File file = new File(dataPath("soundbank-deluxe.gm"));
  
  try {
    soundbank = MidiSystem.getSoundbank(file);
    synthesizer = MidiSystem.getSynthesizer( );
    synthesizer.open();

    channel = synthesizer.getChannels()[1];
  }
  catch (Exception e) { e.printStackTrace(); }
  
  soundbankInstruments = soundbank.getInstruments();
  synthesizer.loadAllInstruments(soundbank);
  synthesizerInstruments = synthesizer.getLoadedInstruments();
  
  // for (int i = 0; i < synthesizerInstruments.length; i++) {
  //   println(i + "  " + synthesizerInstruments[i]);
  // }
  
  channel.programChange(program);
  currentInstrument = synthesizerInstruments[program].toString();
}

void draw(){
  background (127);
  text(program + " " + currentInstrument, 10, height - 8);
  noStroke();
}
void serialEvent(Serial port) {
  String inString = port.readStringUntil('\n');
 
  if (inString != null) {
    inString = trim(inString);
      
    println(inString);
    
    try {
      String[] data = split(inString, ",");
      float x = float (data[0]);  
      xdata[0] = x;

      midiNote = int(map(x, 65, 73, 50, 80));
      
      if (midiNote == prevNote) return;
      
      if (midiNote == -1){
        channel.noteOff(prevNote);
      }
      
      channel.noteOff(prevNote);
      channel.noteOn(midiNote, 64);
      prevNote = midiNote;
      
    redraw(); 
    }
    catch (Exception e) { e.printStackTrace(); }
  }
}
void keyPressed() {
  if (keyCode == 37) {
    program--; if (program == -1) { program = 127; }
    channel.programChange(program);
    currentInstrument = synthesizerInstruments[program].toString();
  }
  else if (keyCode == 39) {
    program++; if (program == 128) { program = 0; }
    channel.programChange(program);
    currentInstrument = synthesizerInstruments[program].toString();
  }
}
