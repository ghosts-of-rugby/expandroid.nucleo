# expandroid.nucleo

st-flash write build/expandroid.nucleo.bin 0x8000000
HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

ping 192.168.137.1