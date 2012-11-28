
/*static*/ ui16 BIOS::KEY::GetKeys()
{
    ui16 status = ~__Get(KEY_STATUS, 0);
    ui16 out=0;
    if (status & K_INDEX_D_STATUS ) out |=KeyLeft;
    if (status & K_INDEX_I_STATUS ) out |=KeyRight;
    if (status & K_INDEX_S_STATUS ) out |=KeyS2;
    if (status & K_ITEM_D_STATUS ) out |=KeyUp;
    if (status & K_ITEM_I_STATUS ) out |=KeyDown;
    if (status & K_ITEM_S_STATUS ) out |=KeyS1;
    if (status & KEY1_STATUS ) out |= KeyEnter;
    if (status & KEY2_STATUS ) out |= KeyEscape;
    if (status & KEY3_STATUS ) out |= KeyFunction;
    if (status & KEY4_STATUS ) out |= KeyFunction2;
	return out;
}
