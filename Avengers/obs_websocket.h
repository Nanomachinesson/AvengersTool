#pragma once

void obsWebsocketConfigure(bool enabled, const char* host, const char* port, const char* password);
void obsWebsocketStartRecord();
void obsWebsocketStopRecord();
void obsWebsocketShutdown();
