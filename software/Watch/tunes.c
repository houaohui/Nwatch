/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2014 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include "common.h"

// Remove const when TUNEMEM is EEPMEM?

const tune_t tuneUsbPlugin[] TUNEMEM = {
	TONE_2KHZ<<16 | 50,
	TONE_PAUSE<<16 | 100,
	TONE_2_5KHZ<<16 | 50,
	TONE_PAUSE<<16 | 100,
	TONE_3KHZ<<16 | 200,
	TONE_STOP
};

const tune_t tuneUsbUnplug[] TUNEMEM = {
	TONE_3KHZ<<16 | 50,
	TONE_PAUSE<<16 | 100,
	TONE_2_5KHZ<<16 | 50,
	TONE_PAUSE<<16 | 100,
	TONE_2KHZ<<16 | 200,
	TONE_STOP
};

const tune_t tuneUsbCharged[] TUNEMEM = {
	TONE_2KHZ<<16 | 150,
	TONE_PAUSE<<16 | 150,
	TONE_2KHZ<<16 | 150,
	TONE_PAUSE<<16 | 150,
	TONE_4KHZ<<16 | 250,
	TONE_4KHZ<<16 | 250,
	TONE_STOP
};

const tune_t tuneHour[] TUNEMEM = {
	TONE_2_5KHZ<<16 | 150,
	TONE_PAUSE<<16 | 80,
	TONE_2_5KHZ<<16 | 150,
	TONE_STOP
};

const tune_t tuneAlarm[] TUNEMEM = {
	TONE_2KHZ<<16 | 100,
	TONE_PAUSE<<16 | 50,
	TONE_2KHZ<<16 | 100,
	TONE_PAUSE<<16 | 50,
	TONE_3KHZ<<16 | 100,
	TONE_PAUSE<<16 | 50,
	TONE_3KHZ<<16 | 100,
	TONE_PAUSE<<16 | 50,
	TONE_REPEAT
};

const tune_t tuneBtn1[] TUNEMEM = {
	TONE_2KHZ<<16 | 100,
	TONE_PAUSE<<16 | 50,
	TONE_3KHZ<<16 | 100,
	TONE_STOP
};

const tune_t tuneBtn2[] TUNEMEM = {
	TONE_3KHZ<<16 | 100,
	TONE_PAUSE<<16 | 50,
	TONE_3KHZ<<16 | 100,
	TONE_STOP
};

const tune_t tuneBtn3[] TUNEMEM = {
	TONE_3KHZ<<16 | 100,
	TONE_PAUSE<<16 | 50,
	TONE_2KHZ<<16 | 100,
	TONE_STOP
};
