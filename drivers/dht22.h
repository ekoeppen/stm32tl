#ifndef __DHT22_H
#define __DHT22_H

template<typename TIMER, typename TIMEOUT, typename DATA_PIN, typename POWER_PIN>
int read_dht(int *temperature, unsigned int *humidity)
{
	// Note: Timer must be continuous mode at 1 MHz
	unsigned char buffer[6];
	unsigned char *p = buffer;
	const unsigned char *end = p + 6;                           // End of data buffer
	unsigned char m = 1;                                        // First byte will have only start bit
	unsigned st, et;                                            // Start and end times

	p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = 0;                // Clear data buffer

	POWER_PIN::set_high();                                           // Turn on
	TIMEOUT::set_and_wait(2000);                                     // Wait 2s
	DATA_PIN::set_low();                                             // Pull low
	DATA_PIN::set_output();                                          // Output
	DATA_PIN::disable_resistor();                                    // Pull low
	TIMEOUT::set_and_wait(18);                                       // Wait 18ms
	DATA_PIN::enable_resistor();                                     // Pull low
	DATA_PIN::set_high();                                            // Pull high
	DATA_PIN::set_input();                                           // Input

	st = TIMER::counter();                                      // Get start time for timeout
	while(DATA_PIN::is_high())
		if((TIMER::counter() - st) > 100) return -1;        // Wait while high, return if no response
	et = TIMER::counter();                                      // Get start time for timeout
	do {
		st = et;                                                // Start time of this bit is end time of previous bit
		while(DATA_PIN::is_low())
			if((TIMER::counter() - st) > 100) return -2;    // Wait while low, return if stuck low
		while(DATA_PIN::is_high())
			if((TIMER::counter() - st) > 200) return -3;    // Wait while high, return if stuck high
		et = TIMER::counter();                                  // Get end time
		if((et - st) > 110) *p |= m;                            // If time > 110 us, then it is a one bit
		if(!(m >>= 1)) m = 0x80, ++p;                           // Shift mask, move to next byte when mask is zero
	} while(p < end);                                           // Do until array is full

	p -= 6;                                                     // Point to start of buffer
	if(p[0] != 1) return -4;                                    // No start bit
	if(((p[1] + p[2] + p[3] + p[4]) & 0xFF) != p[5]) return -5; // Bad checksum

	*temperature = (p[3] << 8) + p[4];
	if (p[3] & 0x80) *temperature = (*temperature & 0x7fff) * -1;
	*humidity = (p[1] << 8) + p[2];

	DATA_PIN::set_output();                                          // Output
	DATA_PIN::set_low();                                             // Pull low
	POWER_PIN::set_low();
	return 0;                                                   // Good read
}

#endif

