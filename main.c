/* ========================================
 *
 * Copyright YOUR COMPANY, 2025
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <string.h>
#include <stdio.h>  // For sprintf

#define PASSWORD_LENGTH 4
#define EEPROM_PASSWORD_ADDR 0x00
#define EEPROM_START_ADDRESS 0x0003E000
#define MAX_WRONG_ATTEMPTS 3
#define LOCKOUT_DURATION 30  // 30 seconds (now in seconds for countdown)

char received_data[PASSWORD_LENGTH + 1];
char stored_password[PASSWORD_LENGTH + 1];
char old_password_input[PASSWORD_LENGTH + 1];
uint8_t data_index = 0;
int is_locked = 1;
int new_pass_mode = 0;
int verify_old_pass = 0;
uint8_t wrong_attempts = 0;
uint8_t in_lockout = 0;

void update_lcd() {
    LCD_Char_ClearDisplay();
    LCD_Char_Position(0, 0);
    if (is_locked) {
        LCD_Char_PrintString("LOCKED");
    } else {
        LCD_Char_PrintString("UNLOCKED");
    }
    
    LCD_Char_Position(1, 0);
    if (in_lockout) {
        // This will be handled in handle_lockout()
    } else if (new_pass_mode) {
        if (verify_old_pass) {
            LCD_Char_PrintString("Old PIN:");
            LCD_Char_Position(1, 8);
            for (uint8_t i = 0; i < data_index; i++) {
                LCD_Char_PutChar('*');
            }
        } else {
            LCD_Char_PrintString("New PIN:");
            LCD_Char_Position(1, 8);
            for (uint8_t i = 0; i < data_index; i++) {
                LCD_Char_PutChar('*');
            }
        }
    } else {
        if (data_index == 0) {
            LCD_Char_PrintString("Enter PIN:");
        } else {
            for (uint8_t i = 0; i < data_index; i++) {
                LCD_Char_PutChar('*');
            }
        }
    }
}

void read_password_from_eeprom() {
    Em_EEPROM_1_Read(EEPROM_PASSWORD_ADDR, (uint8_t*)stored_password, PASSWORD_LENGTH + 1);
    if (stored_password[0] == 0xFF || stored_password[0] == 0x00) {
        strcpy(stored_password, "1234");
        Em_EEPROM_1_Write(EEPROM_PASSWORD_ADDR, (uint8_t*)stored_password, PASSWORD_LENGTH + 1);
    }
}

void write_password_to_eeprom(const char* new_password) {
    Em_EEPROM_1_Write(EEPROM_PASSWORD_ADDR, (uint8_t*)new_password, PASSWORD_LENGTH + 1);
    strcpy(stored_password, new_password);
}

void handle_lockout() {
    in_lockout = 1;
    char buffer[16];  // Buffer for countdown VT (virtual terminal) string formatting
    
    for (int i = LOCKOUT_DURATION; i >= 0; i--) {
        LCD_Char_ClearDisplay();
        LCD_Char_Position(0, 0);
        LCD_Char_PrintString("LOCKED");
        LCD_Char_Position(1, 0);
        sprintf(buffer, "Locked Out: %2d s", i);  // Format remaining seconds
        LCD_Char_PrintString(buffer);
        CyDelay(1000);  // Wait 1 second
    }
    
    in_lockout = 0;
    wrong_attempts = 0;  // Reset wrong attempts after lockout
    update_lcd();
}

int main(void) {
    CyGlobalIntEnable;

    USBUART_1_Start(0u, USBUART_1_3V_OPERATION);
    while (!USBUART_1_bGetConfiguration());
    USBUART_1_CDC_Init();
    
    LCD_Char_Start();
    
    cy_stc_eeprom_config_t eeprom_config = {
        .userFlashStartAddr = EEPROM_START_ADDRESS,
        .eepromSize = Em_EEPROM_1_EEPROM_SIZE,
        .wearLevelingFactor = 2,
        .redundantCopy = 1,
        .blockingWrite = 1
    };
    Em_EEPROM_1_Init(&eeprom_config);
    
    LED_Write(0);
    read_password_from_eeprom();
    update_lcd();

    for(;;) {
        if (in_lockout) {
            continue;  // Skip processing during lockout
        }
        
        if (USBUART_1_DataIsReady()) {
            char c = USBUART_1_GetChar();
            
            if (c >= '0' && c <= '9' && data_index < PASSWORD_LENGTH) {
                if (new_pass_mode && verify_old_pass) {
                    old_password_input[data_index++] = c;
                } else {
                    received_data[data_index++] = c;
                }
                update_lcd();
                
                if (data_index == PASSWORD_LENGTH) {
                    if (new_pass_mode) {
                        if (verify_old_pass) {
                            old_password_input[data_index] = '\0';
                            if (strcmp(old_password_input, stored_password) == 0) {
                                verify_old_pass = 0;
                                data_index = 0;
                                update_lcd();
                            } else {
                                USBUART_1_PutString("W");
                                LCD_Char_Position(1, 0);
                                LCD_Char_PrintString("Wrong Old PIN");
                                CyDelay(2000);
                                new_pass_mode = 0;
                                verify_old_pass = 0;
                                data_index = 0;
                                update_lcd();
                            }
                        } else {
                            received_data[data_index] = '\0';
                            write_password_to_eeprom(received_data);
                            USBUART_1_PutString("S");
                            LCD_Char_Position(1, 0);
                            LCD_Char_PrintString("PIN Set");
                            CyDelay(2000);
                            new_pass_mode = 0;
                            data_index = 0;
                            update_lcd();
                        }
                    }
                }
            }
            else if (c == 'E') {  // Enter pressed
                if (!new_pass_mode && data_index == PASSWORD_LENGTH) {
                    received_data[data_index] = '\0';
                    if (strcmp(received_data, stored_password) == 0) {
                        is_locked = 0;
                        LED_Write(1);
                        USBUART_1_PutString("U");
                        wrong_attempts = 0;  // Reset wrong attempts on success
                        update_lcd();
                        CyDelay(2000);
                        LED_Write(0);
                    } else {
                        is_locked = 1;
                        wrong_attempts++;
                        USBUART_1_PutString("L");
                        LCD_Char_Position(1, 0);
                        LCD_Char_PrintString("Wrong PIN");
                        CyDelay(2000);
                        if (wrong_attempts >= MAX_WRONG_ATTEMPTS) {
                            handle_lockout();
                        }
                    }
                }
                data_index = 0;
                update_lcd();
            }
            else if (c == 'C') {  // Cancel
                data_index = 0;
                new_pass_mode = 0;
                verify_old_pass = 0;
                update_lcd();
            }
            else if (c == 'N') {  // New Pass
                new_pass_mode = 1;
                verify_old_pass = 1;
                data_index = 0;
                update_lcd();
            }
        }
    }
}

/* [] END OF FILE */
