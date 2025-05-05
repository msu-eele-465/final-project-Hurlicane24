#include "intrinsics.h"
#include "msp430fr2355.h"
#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct NFA1 {
    int states[3];
    char alphabet[3];
    char transitions[3][9];
    int start_state;
    int accept_state;
    int epsilon_closures[3][3];
};

struct NFA2 {
    int states[4];
    char alphabet[3];
    char transitions[4][12];
    int start_state;
    int accept_state;
    int epsilon_closures[4][4];
};

struct NFA3 {
    int states[6];
    char alphabet[3];
    int transitions[6][18];
    int start_state;
    int accept_states[2]; 
    int epsilon_closures[6][6];
};

void keypadSetup();
void LEDBarSetup();
void LCDSetup();
void set_RS(int8_t);
void set_RW(int8_t);
void enable_HTL(); 
void set_DB(int8_t, int8_t, int8_t, int8_t, int8_t, int8_t, int8_t, int8_t);
void read_data();
void return_home();
void clear_display();
void displayNFA1Accept();
void displayNFA1Reject();
void displayNFA2Accept();
void displayNFA2Reject();
void displayNFA3Accept();
void displayNFA3Reject();
void displayREGEX1();
void displayREGEX2();
void displayREGEX3();
void master_setup(void);
void write_to_slave(unsigned char);

int i = 0;
int marker = -1;
unsigned char SLAVE_ADDRESS = 0x45;

void setPattern(char[]);
void epsilon_closure_1(int[], int, struct NFA1*, int[]);
void epsilon_closure_3(int[], int, struct NFA3*, int[]);
int accepted1(char*, int, struct NFA1*);
int accepted2(char*, int, struct NFA2*);
int accepted3(char*, int, struct NFA3*);
void keyReleased(volatile unsigned char*, unsigned char);

volatile unsigned char upper_six[] = {48, 48, 48, 48, 48, 48};
int dataCnt = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    master_setup();
    P1DIR &= ~BIT5;
                                            // Disable the GPIO power-on default high-impedance mode
                                            // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;
    UCB0CTLW0 &= ~UCSWRST;                  //Take B0 out of software reset
     __enable_interrupt();

    struct NFA1 nfa1;
    struct NFA2 nfa2;
    struct NFA3 nfa3;
    char alphabet[3] = {'0', '1', 'e'};

    //Initialize states (0-indexed so transitions can be represented easier)
    for(i = 0; i < 3; i++) {
        nfa1.states[i] = i;
    }
    for(i = 0; i < 4; i++) {
        nfa2.states[i] = i;
    }
    for(i = 0; i < 6; i++) {
        nfa3.states[i] = i;
    }

    //Initialize alphabet
    for(i = 0; i < 3; i++) {
        nfa1.alphabet[i] = alphabet[i];
        nfa2.alphabet[i] = alphabet[i];
        nfa3.alphabet[i] = alphabet[i];
    }

    //Initialize start states
    nfa1.start_state = 0;
    nfa2.start_state = 0;
    nfa3.start_state = 0;

    //Initialize accept states
    nfa1.accept_state = 0;
    nfa2.accept_state = 3;
    nfa3.accept_states[0] = 2;
    nfa3.accept_states[1] = 4;

    //Initialize nfa1 transitions (verified)
    for(i = 0; i < 9; i++) {
        if(i == 4 || i == 8) {
            nfa1.transitions[0][i] = 1;
        }
        else {
            nfa1.transitions[0][i] = 0;
        }
    }
    for(i = 0; i < 9; i++) {
        if(i == 3 || i == 6 || i == 7) {
            nfa1.transitions[1][i] = 1;
        }
        else {
            nfa1.transitions[1][i] = 0;
        }
    }
    for(i = 0; i < 9; i++) {
        if(i == 0) {
            nfa1.transitions[2][i] = 1;
        }
        else{
            nfa1.transitions[2][i] = 0;
        }
    }

    //Initialize nfa2 transitions (verified)
    for(i = 0; i < 12; i++) {
        if(i == 3 || i == 7) {
            nfa2.transitions[0][i] = 1;
        }
        else {
            nfa2.transitions[0][i] = 0;
        }
    }
    for(i = 0; i < 12; i++) {
        if(i == 4 || i == 9) {
            nfa2.transitions[1][i] = 1;
        }
        else {
            nfa2.transitions[1][i] = 0;
        }
    }
    for(i = 0; i < 12; i++) {
        if(i == 6 || i == 10) {
            nfa2.transitions[2][i] = 1;
        }
        else {
            nfa2.transitions[2][i] = 0;
        }
    }
    for(i = 0; i < 12; i++) {
        nfa2.transitions[3][i] = 0;
    }

    //Initialize nfa3 transitions (verified)
    for(i = 0; i < 18; i++) {
        if(i == 3 || i == 5 || i == 7) {
            nfa3.transitions[0][i] = 1;
        }
        else {
            nfa3.transitions[0][i] = 0;
        }
    }
    for(i = 0; i < 18; i++) {
        if(i == 10) {
            nfa3.transitions[1][i] = 1;
        }
        else {
            nfa3.transitions[1][i] = 0;
        }
    }
    for(i = 0; i < 18; i++) {
        if(i == 3 || i == 11) {
            nfa3.transitions[2][i] = 1;
        }
        else {
            nfa3.transitions[2][i] = 0;
        }
    }
    for(i = 0; i < 18; i++) {
        if(i == 12 || i == 13 || i == 16 || i == 17) {
            nfa3.transitions[3][i] = 1;
        }
        else {
            nfa3.transitions[3][i] = 0;
        }
    }
    for(i = 0; i < 18; i++) {
        if(i == 15) {
            nfa3.transitions[4][i] = 1;
        }
        else {
            nfa3.transitions[4][i] = 0;
        }
    }
    for(i = 0; i < 18; i++) {
        nfa3.transitions[5][i] = 0;
    }

     //Initialize variables
    char string[16] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
    bool nfa_select = true;
    bool string_selected = false;
    bool running = true;
    int selected_nfa = 0;
    int accepted = 0;
    unsigned int counter = 0;

    keypadSetup();
    LEDBarSetup();
    LCDSetup();

    setPattern(string);

    //Poll keypad forever
    while (true) 
    {
        //poll row 1
        P3OUT &= ~BIT1;
        if ((P5IN & BIT0) == 0) 
        {
            if(nfa_select == true) {
                selected_nfa = 1;
                nfa_select = false;
            }
            else {
                string[counter] = '1';
                counter++;
            }
            keyReleased(&P5IN, BIT0);           
        } 
        //If selecting NFA, allow nfa selection characters
        if(nfa_select == true)
        {
            if ((P5IN & BIT1) == 0) 
            {
                selected_nfa = 2;
                nfa_select = false;
                keyReleased(&P5IN, BIT1);
            } 
            else if ((P5IN & BIT3) == 0) 
            {
                selected_nfa = 3;
                nfa_select = false;
                keyReleased(&P5IN, BIT3);
            } 
        }       
        P3OUT |= BIT1;

        //poll row 4
        P5OUT &= ~BIT4;
        if ((P5IN & BIT1) == 0) 
        {
            string[counter] = '0';
            counter++;
            keyReleased(&P5IN, BIT1);
        } 
        //If # is selected, string has been selected
        else if ((P5IN & BIT3) == 0) 
        {
            string_selected = true;
            keyReleased(&P5IN, BIT3);
        } 
        P5OUT |= BIT4;
        
        if(string_selected == true) 
        {   
            for(i = 0; i < 6; i++) {
                upper_six[i] = string[i+10];
            }
            write_to_slave(SLAVE_ADDRESS);
            
            setPattern(string);
            if(selected_nfa == 1) {
                accepted = accepted1(string, counter, &nfa1);
                if(accepted == 1)
                {
                    displayNFA1Accept();
                    marker = 1;
                }
                else 
                {
                    displayNFA1Reject();
                    marker = 2;
                }
            }
            else if(selected_nfa == 2) {
                accepted = accepted2(string, counter, &nfa2);
                if(accepted == 1)
                {
                    displayNFA2Accept();
                    marker = 3;
                }
                else 
                {
                    displayNFA2Reject();
                    marker = 4;
                }
            }
            else {
                accepted = accepted3(string, counter, &nfa3);
                if(accepted == 1)
                {
                    displayNFA3Accept();
                    marker = 5;
                }
                else 
                {
                    displayNFA3Reject();
                    marker = 6;
                }
            }
            counter = 0;
            string_selected = false;
            P5OUT &= ~BIT4;
            while(running == true) 
            {
                if((P1IN & BIT5)) {
                    clear_display();
                    return_home();
                    if(marker == 1 || marker == 2) {
                        displayREGEX1();
                    }
                    else if(marker == 3 || marker == 4) {
                        displayREGEX2();
                    }
                    else {
                        displayREGEX3();
                    }
                    while((P1IN & BIT5)) {

                    }
                    clear_display();
                    return_home();
                    if(marker == 1) {
                        displayNFA1Accept();
                    }
                    else if(marker == 2) {
                        displayNFA1Reject();
                    }
                    else if(marker == 3) {
                        displayNFA2Accept();
                    }
                    else if(marker == 4) {
                        displayNFA2Reject();
                    }
                    else if(marker == 5) {
                        displayNFA3Accept();
                    }
                    else {
                        displayNFA3Reject();
                    }
                }
                if((P5IN & BIT0) == 0)
                {
                    running = false;
                    keyReleased(&P5IN, BIT0);
                    for(i = 0; i < 16; i++) 
                    {
                        string[i] = '0';
                        if(i >= 10) {
                            upper_six[i-10] = '0';
                        }
                    }
                    write_to_slave(SLAVE_ADDRESS);
                    setPattern(string);
                    selected_nfa = 0;
                    nfa_select = true;
                    accepted = 0;
                    marker = -1;
                    clear_display();
                    return_home();
                    P5OUT |= BIT0;
                }
            }
            running = true;            
        }
    }

    return(0);
}

//Setup function for keypad
void keypadSetup() 
{
    P1DIR &= ~BIT4;         // all columns set as inputs
    P5DIR &= ~BIT3;         //C4, C3, C2, C1
    P5DIR &= ~BIT1;
    P5DIR &= ~BIT0;

    P1REN |= BIT4;          // enable resistors for columns
    P5REN |= BIT3;
    P5REN |= BIT1;
    P5REN |= BIT0;
    P1OUT |= BIT4;          // set column resistors as pull-ups
    P5OUT |= BIT3;
    P5OUT |= BIT1;
    P5OUT |= BIT0;

    P5DIR |= BIT4;          // all rows set as outputs
    P1DIR |= BIT1;
    P3DIR |= BIT5;
    P3DIR |= BIT1;

    P5OUT |= BIT4;          // initialize row outputs HI
    P1OUT |= BIT1;          //R4, R3, R2, R1
    P3OUT |= BIT5;
    P3OUT |= BIT1;
}

void LEDBarSetup() {
    //LED bar 1
    P2DIR |= BIT0;
    P2DIR |= BIT2;
    P4DIR |= BIT0;
    P4DIR |= BIT6;
    P4DIR |= BIT7;
    P4DIR |= BIT4;
    P2DIR |= BIT5;
    P3DIR |= BIT0;
    P2DIR |= BIT1;
    P6DIR |= BIT0;
}

void LCDSetup()
{
    P4DIR |= BIT5;
    P5DIR |= BIT2;
    P3DIR |= BIT6 | BIT7;
    P1DIR |= BIT6 | BIT7;
    P2DIR |= BIT4;
    P6DIR |= BIT1 | BIT2 | BIT3 | BIT4;

    // Set 8-bit mode
    set_RS(0);
    set_RW(0);
    enable_HTL();
    set_DB(0, 0, 0, 1, 1, 1, 0, 0);
    __delay_cycles(2000);

    // Turn on display, cursor off, not blinking
    set_RS(0);
    set_RW(0);
    enable_HTL();
    set_DB(0, 0, 1, 1, 0, 0, 0, 0);
    __delay_cycles(2000);

    // Clear all characters and reset DDRAM address to 0
    set_RS(0);
    set_RW(0);
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 0, 0);
    __delay_cycles(2000);

    // Set cursor to move right, no display shift
    set_RS(0);
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 1, 0, 0, 0, 0, 0);
    __delay_cycles(2000);
}

void set_RS(int8_t bit) {
    if(bit)
    {
        P6OUT |= BIT1;
    } 
    else 
    {
        P6OUT &= ~BIT1;
    }
}

void set_RW(int8_t bit) {
    if (bit) P6OUT |= BIT2;
    else P6OUT &= ~BIT2;
}

void enable_HTL() 
{
    P6OUT |= BIT3;
    __delay_cycles(5);
    P6OUT &= ~BIT3;
}

void set_DB(int8_t bit0, int8_t bit1, int8_t bit2, int8_t bit3, int8_t bit4, int8_t bit5, int8_t bit6, int8_t bit7) 
{
    if (bit0) P6OUT |= BIT4;
    else P6OUT &= ~BIT4;

    if (bit1) P3OUT |= BIT7;
    else P3OUT &= ~BIT7;

    if (bit2) P2OUT |= BIT4;
    else P2OUT &= ~BIT4;

    if (bit3) P1OUT |= BIT6;
    else P1OUT &= ~BIT6;

    if (bit4) P1OUT |= BIT7;
    else P1OUT &= ~BIT7;

    if (bit5) P3OUT |= BIT6;
    else P3OUT &= ~BIT6;

    if (bit6) P5OUT |= BIT2;
    else P5OUT &= ~BIT2;

    if (bit7) P4OUT |= BIT5;
    else P4OUT &= ~BIT5;
}

void read_data() 
{
    set_RS(1);
    set_RW(0);
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    set_RS(0);
}

void return_home() 
{
    set_RS(0);
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 0, 0, 0, 0, 0, 0);
    __delay_cycles(2000);
}

void clear_display() 
{
    set_RS(0);
    set_RW(0);
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 0, 0);
    __delay_cycles(2000);
}

void setPattern(char pattern[]) 
{
    //LED bar 1
    if(pattern[0] == '1') {
        P2OUT |= BIT0;
    }
    else {
        P2OUT &= ~BIT0;
    }
    if(pattern[1] == '1') {
        P2OUT |= BIT2;
    }
    else {
        P2OUT &= ~BIT2;
    }
    if(pattern[2] == '1') {
        P4OUT |= BIT0;
    }
    else {
        P4OUT &= ~BIT0;
    }
    if(pattern[3] == '1') {
        P4OUT |= BIT6;
    }
    else {
        P4OUT &= ~BIT6;
    }
    if(pattern[4] == '1') {
        P4OUT |= BIT7;
    }
    else {
        P4OUT &= ~BIT7;
    }
    if(pattern[5] == '1') {
        P4OUT |= BIT4;
    }
    else {
        P4OUT &= ~BIT4;
    }
    if(pattern[6] == '1') {
        P2OUT |= BIT5;
    }
    else {
        P2OUT &= ~BIT5;
    }
    if(pattern[7] == '1') {
        P3OUT |= BIT0;
    }
    else {
        P3OUT &= ~BIT0;
    }
    if(pattern[8] == '1') {
        P2OUT |= BIT1;
    }
    else {
        P2OUT &= ~BIT1;
    }
    if(pattern[9] == '1') {
        P6OUT |= BIT0;
    }
    else {
        P6OUT &= ~BIT0;
    }
}

//Finds the epsilon closure of a set of states in NFA 1 (verified)
void epsilon_closure_1(int states[], int size, struct NFA1 *nfa1, int closure[]) {
    int i = 0;
    int j = 0;
    int closure_found = 0;
    int current_closure[3] = {0, 0, 0};
    int new_closure[3] = {0, 0, 0};
    int local_closure[3] = {0, 0, 0};

    //Every member of states will be in the epsilon closure
    for(i = 0; i < size; i++) {
        current_closure[states[i]] = 1;
        new_closure[states[i]] = 1;
    }

    for(i = 0; i < 3; i++) {
        if(current_closure[i] != 1) {
            current_closure[i] = 0;
            new_closure[i] = 0;
        }
    }

    while(closure_found != 1) {
        closure_found = 1;
        for(i = 0; i < 3; i++) {
            if(current_closure[i] == 1) {
                local_closure[i] = 1;
                for(j = 0; j < 9; j++) {
                    if(j % 3 == 2) {
                        if(nfa1->transitions[i][j] == 1) {
                            local_closure[j/3] = 1;
                        }
                    }
                }
                for(j = 0; j < 3; j++) {
                    if(local_closure[j] == 1) {
                        new_closure[j] = 1;
                    }
                }
                for(j = 0; j < 3; j++) {
                    local_closure[j] = 0;
                }
            }
        }
        for(i = 0; i < 3; i++) {
            if(current_closure[i] != new_closure[i]) {
                closure_found = 0;
            }
        }
        for(i = 0; i < 3; i++) {
            current_closure[i] = new_closure[i];
        }
    }

    for(i = 0; i < 3; i++) {
        closure[i] = current_closure[i];
    }
}

//Finds the epsilon closure of a set of states in NFA 3 (verified)
void epsilon_closure_3(int states[], int size,  struct NFA3 *nfa3, int closure[]) {
    int i = 0;
    int j = 0;
    int closure_found = 0;
    int current_closure[6] = {0, 0, 0, 0, 0, 0};
    int new_closure[6] = {0, 0, 0, 0, 0, 0};
    int local_closure[6] = {0, 0, 0, 0, 0, 0};

    //Every member of states will be in the epsilon closure
    for(i = 0; i < size; i++) {
        current_closure[states[i]] = 1;
        new_closure[states[i]] = 1;
    }

    for(i = 0; i < 6; i++) {
        if(current_closure[i] != 1) {
            current_closure[i] = 0;
            new_closure[i] = 0;
        }
    }

    while(closure_found != 1) {
        closure_found = 1;
        for(i = 0; i < 6; i++) {
            if(current_closure[i] == 1) {
                local_closure[i] = 1;
                for(j = 0; j < 18; j++) {
                    if(j % 3 == 2) {
                        if(nfa3->transitions[i][j] == 1) {
                            local_closure[j/3] = 1;
                        }
                    }
                }
                for(j = 0; j < 6; j++) {
                    if(local_closure[j] == 1) {
                        new_closure[j] = 1;
                    }
                }
                for(j = 0; j < 6; j++) {
                    local_closure[j] = 0;
                }
            }
        }
        for(i = 0; i < 6; i++) {
            if(current_closure[i] != new_closure[i]) {
                closure_found = 0;
            }
        }
        for(i = 0; i < 6; i++) {
            current_closure[i] = new_closure[i];
        }
    }

    for(i = 0; i < 6; i++) {
        closure[i] = current_closure[i];
    }
}

//Determines if a string is accepted or rejected by NFA 1 (verified)
int accepted1(char *string, int size, struct NFA1 *nfa1) {
    int i = 0;
    int j = 0;
    int k = 0;
    int epsilon_closure[3] = {0, 0, 0};
    int reachable[3] = {1, 0, 1};
    int new_reachable[3] = {0, 0, 0};
    int states1[1];
    int states2[2];
    int states3[3];
    int states = 0;
    int state_index = 0;
    int index_modulus;

    for(i = 0; i < size; i++) {
        char current_char = string[i];
        if(current_char == '0') {
            index_modulus = 0;
        }
        else {
            index_modulus = 1;
        }

        //Get epsilon closure of current states
        for(j = 0; j < 3; j++) {
            if(reachable[j] == 1) {
                states++;
            }
        }
        if(states == 0) {
            for(j = 0; j < 3; j++) {
                epsilon_closure[j] = 0;
            }
        }
        else if(states == 1) {
            for(j = 0; j < 3; j++) {
                if (reachable[j] == 1) {
                    states1[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_1(states1, states, nfa1, epsilon_closure);
        }
        else if(states == 2) {
            for(j = 0; j < 3; j++) {
                if (reachable[j] == 1) {
                    states2[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_1(states2, states, nfa1, epsilon_closure);
        }
        else {
            for(j = 0; j < 3; j++) {
                if (reachable[j] == 1) {
                    states3[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_1(states3, states, nfa1, epsilon_closure);
        }
        states = 0;
        state_index = 0;

        //Set current states to epsilon closure of current states
        for(j = 0; j < 3; j++) {
            reachable[j] = epsilon_closure[j];
        }

        //Take transitions
        for(j = 0; j < 3; j++) {
            if(reachable[j] == 1) {
                for(k = 0; k < 9; k++) {
                    if(k % 3 == index_modulus && nfa1->transitions[j][k] == 1) {
                        new_reachable[k/3] = 1;
                    }
                }
            }            
        }
        for(j = 0; j < 3; j++) {
            reachable[j] = new_reachable[j];
            new_reachable[j] = 0;
        }
    }
    if(reachable[0] == 1) {
        return(1);
    }
    else {
        return(0);
    }
}

//Determines if a string is accepted or rejected by NFA 2 (verified)
int accepted2(char *string, int size, struct NFA2 *nfa2) {
    int i = 0;
    int j = 0;
    int k = 0;
    int reachable[4] = {1, 0, 0, 0};
    int new_reachable[4] = {0, 0, 0, 0};
    int index_modulus;

    for(i = 0; i < size; i++) {
        char current_char = string[i];     
        if(current_char == '0') {
            index_modulus = 0;
        }
        else {
            index_modulus = 1;
        }
        for(j = 0; j < 4; j++) {
            if(reachable[j] == 1) {
                for(k = 0; k < 12; k++) {
                    if(k % 3 == index_modulus && nfa2->transitions[j][k] == 1) {
                        new_reachable[k/3] = 1;
                    }
                }
            }
        }
        for(j = 0; j < 4; j++) {
            reachable[j] = new_reachable[j];
            new_reachable[j] = 0;
        }
    }
    if(reachable[3] == 1) {
        return(1);
    }
    else {
        return(0);
    }
}

//Determines if a string is accepted or rejected by NFA 3 (verified)
int accepted3(char *string, int size, struct NFA3 *nfa3) {
    int i = 0;
    int j = 0;
    int k = 0;
    int epsilon_closure[6] = {0, 0, 0, 0, 0, 0};
    int reachable[6] = {1, 1, 0, 0, 0, 0};
    int new_reachable[6] = {0, 0, 0, 0, 0, 0};
    int states1[1];
    int states2[2];
    int states3[3];
    int states4[4];
    int states5[5];
    int states6[6];
    int states = 0;
    int state_index = 0;
    int index_modulus;

    for(i = 0; i < size; i++) {
        char current_char = string[i];
        if(current_char == '0') {
            index_modulus = 0;
        }
        else {
            index_modulus = 1;
        }

        //Get epsilon closure of current states
        for(j = 0; j < 6; j++) {
            if(reachable[j] == 1) {
                states++;
            }
        }
        if(states == 0) {
            for(j = 0; j < 6; j++) {
                epsilon_closure[j] = 0;
            }
        }
        else if(states == 1) {
            for(j = 0; j < 6; j++) {
                if (reachable[j] == 1) {
                    states1[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_3(states1, states, nfa3, epsilon_closure);
        }
        else if(states == 2) {
            for(j = 0; j < 6; j++) {
                if (reachable[j] == 1) {
                    states2[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_3(states2, states, nfa3, epsilon_closure);
        }
        else if (states == 3) {
            for(j = 0; j < 6; j++) {
                if (reachable[j] == 1) {
                    states3[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_3(states3, states, nfa3, epsilon_closure);
        }
        else if (states == 4) {
            for(j = 0; j < 6; j++) {
                if (reachable[j] == 1) {
                    states4[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_3(states4, states, nfa3, epsilon_closure);
        }
        else if (states == 5) {
            for(j = 0; j < 6; j++) {
                if (reachable[j] == 1) {
                    states5[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_3(states5, states, nfa3, epsilon_closure);
        }
        else {
            for(j = 0; j < 6; j++) {
                if (reachable[j] == 1) {
                    states6[state_index] = j;
                    state_index++;
                }
            }
            epsilon_closure_3(states6, states, nfa3, epsilon_closure);
        }
        states = 0;
        state_index = 0;

        //Set current states to epsilon closure of current states
        for(j = 0; j < 6; j++) {
            reachable[j] = epsilon_closure[j];
        }
        for(j = 0; j < 6; j++) {
        }

        //Take transitions
        for(j = 0; j < 6; j++) {
            if(reachable[j] == 1) {
                for(k = 0; k < 18; k++) {
                    if(k % 3 == index_modulus && nfa3->transitions[j][k] == 1) {
                        new_reachable[k/3] = 1;
                    }
                }
            }            
        }
        for(j = 0; j < 6; j++) {
            reachable[j] = new_reachable[j];
            new_reachable[j] = 0;
        }
    }
    if(reachable[2] == 1 || reachable[4] == 1) {
        return(1);
    }
    else {
        return(0);
    }
}

void displayNFA1Accept()
{
    set_RS(0);  //N
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 1, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //F
    enable_HTL();
    set_DB(0, 1, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //:
    enable_HTL();
    set_DB(0, 1, 0, 1, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //P
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //T
    enable_HTL();
    set_DB(0, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //D
    enable_HTL();
    set_DB(0, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();
}

void displayNFA1Reject()
{
    set_RS(0);  //N
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 1, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //F
    enable_HTL();
    set_DB(0, 1, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //:
    enable_HTL();
    set_DB(0, 1, 0, 1, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //R
    enable_HTL();
    set_DB(0, 1, 0, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //J
    enable_HTL();
    set_DB(0, 1, 0, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //T
    enable_HTL();
    set_DB(0, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //D
    enable_HTL();
    set_DB(0, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();
}

void displayNFA2Accept()
{
    set_RS(0);  //N
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 1, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //F
    enable_HTL();
    set_DB(0, 1, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //2
    enable_HTL();
    set_DB(0, 1, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //:
    enable_HTL();
    set_DB(0, 1, 0, 1, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //P
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //T
    enable_HTL();
    set_DB(0, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //D
    enable_HTL();
    set_DB(0, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();
}

void displayNFA2Reject()
{
    set_RS(0);  //N
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 1, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //F
    enable_HTL();
    set_DB(0, 1, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //2
    enable_HTL();
    set_DB(0, 1, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //:
    enable_HTL();
    set_DB(0, 1, 0, 1, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //R
    enable_HTL();
    set_DB(0, 1, 0, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //J
    enable_HTL();
    set_DB(0, 1, 0, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //T
    enable_HTL();
    set_DB(0, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //D
    enable_HTL();
    set_DB(0, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();
}

void displayNFA3Accept()
{
    set_RS(0);  //N
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 1, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //F
    enable_HTL();
    set_DB(0, 1, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //3
    enable_HTL();
    set_DB(1, 1, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //:
    enable_HTL();
    set_DB(0, 1, 0, 1, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //P
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //T
    enable_HTL();
    set_DB(0, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //D
    enable_HTL();
    set_DB(0, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();
}

void displayNFA3Reject()
{
    set_RS(0);  //N
    set_RW(0);
    enable_HTL();
    set_DB(0, 1, 1, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //F
    enable_HTL();
    set_DB(0, 1, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //A
    enable_HTL();
    set_DB(1, 0, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //3
    enable_HTL();
    set_DB(1, 1, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //:
    enable_HTL();
    set_DB(0, 1, 0, 1, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //Space
    enable_HTL();
    set_DB(0, 0, 0, 0, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //R
    enable_HTL();
    set_DB(0, 1, 0, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //J
    enable_HTL();
    set_DB(0, 1, 0, 1, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //C
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //T
    enable_HTL();
    set_DB(0, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //E
    enable_HTL();
    set_DB(1, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //D
    enable_HTL();
    set_DB(0, 0, 1, 0, 0, 0, 1, 0);
    __delay_cycles(2000);
    read_data();
}

void displayREGEX1()
{
    set_RS(0);  //0
    set_RW(0);
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //*
    enable_HTL();
    set_DB(0, 1, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //(
    enable_HTL();
    set_DB(0, 0, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //0
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //0
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //+
    enable_HTL();
    set_DB(1, 1, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //)
    enable_HTL();
    set_DB(1, 0, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //+
    enable_HTL();
    set_DB(1, 1, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //(
    enable_HTL();
    set_DB(0, 0, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //0
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //*
    enable_HTL();
    set_DB(0, 1, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();
}

void displayREGEX2()
{   
    set_RS(0);  //0
    set_RW(0);  
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //*
    enable_HTL();
    set_DB(0, 1, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //0 
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //0 
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //*
    enable_HTL();
    set_DB(0, 1, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();
}

void displayREGEX3()
{
    set_RS(0);  //1
    set_RW(0);
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //0 
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();
    
    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //(
    enable_HTL();
    set_DB(0, 0, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //0 
    enable_HTL();
    set_DB(0, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //1
    enable_HTL();
    set_DB(1, 0, 0, 0, 1, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //)
    enable_HTL();
    set_DB(1, 0, 0, 1, 0, 1, 0, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //epsilon
    enable_HTL();
    set_DB(1, 1, 0, 0, 0, 1, 1, 1);
    __delay_cycles(2000);
    read_data();

    set_RW(0);  //U
    enable_HTL();
    set_DB(1, 0, 1, 0, 1, 0, 1, 0);
    __delay_cycles(2000);
    read_data();
}

void master_setup()
{
    UCB0CTLW0 |= UCSWRST;               //Software Reset

    UCB0CTLW0 |= UCSSEL__SMCLK;         //SMCLK
    UCB0BRW = 10;                       //Set prescalar to 10

    UCB0CTLW0 |= UCMODE_3;              //Put into i2c mode
    UCB0CTLW0 |= UCMST;                 //Set MSP430FR2355 as master

    UCB0CTLW1 |= UCASTP_2;
    UCB0TBCNT = 0x06;

    P1SEL1 &= ~BIT3;                    //SCL setup
    P1SEL0 |= BIT3;

    P1SEL1 &= ~BIT2;                    //SDA setup
    P1SEL0 |= BIT2;
}

void write_to_slave(unsigned char slave_address)
{   
    UCB0I2CSA = slave_address;
    UCB0CTLW0 |= UCTR;
    UCB0TBCNT = 6;
    UCB0IE |= UCTXIE0;
    UCB0CTLW0 |= UCTXSTT;
    for (i = 0; i < 800; i++)
    {

    }   
    UCB0IE &= ~UCTXIE0;
    UCB0CTLW0 &= ~UCTR;

}

//Check if key has been unpressed
void keyReleased(volatile unsigned char* pin, unsigned char bit) 
{
    while ((*pin & bit) == 0) {}
    __delay_cycles(200000);
    return;
}

//Master ISR
#pragma vector=EUSCI_B0_VECTOR
__interrupt void EUSCI_B0_I2C_ISR(void)
{
    UCB0TXBUF = upper_six[dataCnt];
    dataCnt++;
    if(dataCnt == 6) {
        dataCnt = 0;
    }
}
