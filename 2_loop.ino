void loop() 
{
	Mode* mode = modeSelector.getMode();
	if (mode != NULL) mode->loop();
}
