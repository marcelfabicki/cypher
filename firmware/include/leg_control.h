#pragma once

void setLegPositions (float xInput, float yInput);
void initialiseServos();
void setLegAngle(float upper, float lower);
void homeServos();
void setTimedLegPosition (float x, float y, int time);