import pyaudio
import speech_recognition as sr
import sys

#read duration from the arguments
duration = int(sys.argv[1])


r = sr.Recognizer()
with sr.Microphone() as source:
    # read the audio data from the default microphone
    print("speak now")
    audio_data = r.record(source, duration=duration)
    print("Recognizing...")
    # convert speech to text
    text = r.recognize_google(audio_data)
    print(text)