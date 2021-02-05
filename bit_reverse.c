static uint8_t reverse_mask(uint8_t x)
{
	x = ((x & 0x55) << 1) | ((x & 0xAA) >> 1);
	x = ((x & 0x33) << 2) | ((x & 0xCC) >> 2);
	x = ((x & 0x0F) << 4) | ((x & 0xF0) >> 4);					// for uint8_t
	//x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8)		// for uint16_t
	//x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16)	// for uint32_t
	return x;
}
