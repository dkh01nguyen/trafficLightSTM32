# Traffic Light System - Finite State Machine (FSM) Documentation

## Overview

This document describes the Finite State Machine (FSM) for the traffic light control system implemented on STM32F103RB microcontroller. The system supports both automatic and manual operation modes with configuration capabilities.

---

## System States

The FSM consists of **8 main states**:

### 1. **STATE_INIT** (Initialization)

- **Description**: Initial startup state displaying welcome message
- **Duration**: 3 seconds
- **Display**: "HCMUT PROJECT / TRAFFIC LIGHT"
- **Light Behavior**: All lights OFF
- **Transition**: Automatically transitions to `STATE_AUTO_NORM` after 3 seconds

### 2. **STATE_AUTO_NORM** (Auto Normal Operation)

- **Description**: Normal automatic traffic light operation
- **Light Behavior**:
  - Cycles through 4 phases if balanced (R = Y + G):
    - NS: Green ↔ EW: Red
    - NS: Yellow ↔ EW: Red
    - NS: Red ↔ EW: Green
    - NS: Red ↔ EW: Yellow
  - If unbalanced: All lights flash YELLOW (error indication)
- **Display**: Shows countdown timers or error message
- **Transitions**:
  - `BUTTON_1_MOD1` → `STATE_MANUAL`
  - `BUTTON_2_MOD1` → `STATE_AUTO_RED`

### 3. **STATE_AUTO_RED** (Red Duration Configuration)

- **Description**: Configuration mode for RED light duration
- **Light Behavior**: Both directions flash RED
- **Display**: "CONFIG RED / R:xx Y:xx G:xx"
- **Controls**:
  - `BUTTON_1_MOD2`: Increment RED duration (1-99 seconds)
  - `BUTTON_2_MOD2`: Decrement RED duration (1-99 seconds)
- **Transitions**:
  - `BUTTON_1_MOD1` → `STATE_MANUAL`
  - `BUTTON_2_MOD1` → `STATE_AUTO_YEL`

### 4. **STATE_AUTO_YEL** (Yellow Duration Configuration)

- **Description**: Configuration mode for YELLOW light duration
- **Light Behavior**: Both directions flash YELLOW
- **Display**: "CONFIG YELLOW / R:xx Y:xx G:xx"
- **Controls**:
  - `BUTTON_1_MOD2`: Increment YELLOW duration (1-99 seconds)
  - `BUTTON_2_MOD2`: Decrement YELLOW duration (1-99 seconds)
- **Transitions**:
  - `BUTTON_1_MOD1` → `STATE_MANUAL`
  - `BUTTON_2_MOD1` → `STATE_AUTO_GRN`

### 5. **STATE_AUTO_GRN** (Green Duration Configuration)

- **Description**: Configuration mode for GREEN light duration
- **Light Behavior**: Both directions flash GREEN
- **Display**: "CONFIG GREEN / R:xx Y:xx G:xx"
- **Controls**:
  - `BUTTON_1_MOD2`: Increment GREEN duration (1-99 seconds)
  - `BUTTON_2_MOD2`: Decrement GREEN duration (1-99 seconds)
- **Transitions**:
  - `BUTTON_1_MOD1` → `STATE_MANUAL`
  - `BUTTON_2_MOD1` → `STATE_AUTO_NORM` (with balance check)

### 6. **STATE_MANUAL** (Manual Operation)

- **Description**: Manual control of traffic lights
- **Light Behavior**: Static lights based on sub-state
  - `MANUAL_NS_RED_EW_GREEN`: NS=Red, EW=Green
  - `MANUAL_NS_GREEN_EW_RED`: NS=Green, EW=Red
- **Display**: "OPR: MANUAL / NS:X EW:Y"
- **Controls**:
  - `BUTTON_2_MOD1`: Toggle between manual sub-states
  - `BUTTON_1_MOD2`: Enter `STATE_MANUAL_FLASH_YEL`
  - `BUTTON_2_MOD2`: Enter `STATE_MANUAL_FLASH_RED`
- **Transitions**:
  - `BUTTON_1_MOD1` → `STATE_AUTO_NORM`
  - `BUTTON_1_MOD2` → `STATE_MANUAL_FLASH_YEL`
  - `BUTTON_2_MOD2` → `STATE_MANUAL_FLASH_RED`

### 7. **STATE_MANUAL_FLASH_YEL** (Manual Yellow Flash)

- **Description**: Manual flash mode with yellow lights
- **Light Behavior**: Both directions flash YELLOW
- **Display**: "OPR: MANUAL / FLASH YEL"
- **Transitions**:
  - `BUTTON_1_MOD1` → `STATE_AUTO_NORM`
  - `BUTTON_1_MOD2` → `STATE_MANUAL` (default sub-state)

### 8. **STATE_MANUAL_FLASH_RED** (Manual Red Flash)

- **Description**: Manual flash mode with red lights
- **Light Behavior**: Both directions flash RED
- **Display**: "OPR: MANUAL / FLASH RED"
- **Transitions**:
  - `BUTTON_1_MOD1` → `STATE_AUTO_NORM`
  - `BUTTON_2_MOD2` → `STATE_MANUAL` (default sub-state)

---

## State Transition Diagram

```plain
                                    [POWER ON]
                                        ↓
                            ┌───────────────────────┐
                            │    STATE_INIT         │
                            │  (Welcome Screen)     │
                            │  Duration: 3 seconds  │
                            └───────────────────────┘
                                        ↓ [After 3s]
                                        ↓
         ┌──────────────────────────────────────────────────────────┐
         │                  AUTO MODE GROUP                         │
         │  ┌────────────────────────────────────────────────────┐  │
         │  │                                                    │  │
         │  │     ┌─────────────────────┐                        │  │
         │  │     │  STATE_AUTO_NORM    │◄───────────┐           │  │
         │  │     │ (Normal Operation)  │            │           │  │
         │  │     └─────────────────────┘            │           │  │
         │  │              │                         │           │  │
         │  │              │ BTN_2_MOD1              │           │  │
         │  │              ↓                         │           │  │
         │  │     ┌─────────────────────┐            │           │  │
         │  │     │  STATE_AUTO_RED     │            │           │  │
         │  │  ┌──│  (Config Red)       │            │           │  │
         │  │  │  └─────────────────────┘            │           │  │
         │  │  │           │                         │           │  │
         │  │  │           │ BTN_2_MOD1              │           │  │
         │  │  │           ↓                         │           │  │
         │  │  │  ┌─────────────────────┐            │           │  │
         │  │  │  │  STATE_AUTO_YEL     │            │           │  │
         │  │  │  │  (Config Yellow)    │            │           │  │
         │  │  │  └─────────────────────┘            │           │  │
         │  │  │           │                         │           │  │
         │  │  │           │ BTN_2_MOD1              │           │  │
         │  │  │           ↓                         │           │  │
         │  │  │  ┌─────────────────────┐            │           │  │
         │  │  │  │  STATE_AUTO_GRN     │            │           │  │
         │  │  └─►│  (Config Green)     │────────────┘           │  │
         │  │     └─────────────────────┘   BTN_2_MOD1           │  │
         │  │                                                    │  │
         │  └────────────────────────────────────────────────────┘  │
         │           │                                 ▲            │
         └───────────┼─────────────────────────────────┼────────────┘
                     │ BTN_1_MOD1                      │ BTN_1_MOD1
                     │                                 │
         ┌───────────┼─────────────────────────────────┼──────────────┐
         │           │            MANUAL MODE GROUP    │              │
         │           ▼                                 │              │
         │  ┌─────────────────────┐                    │              │
         │  │   STATE_MANUAL      │                    │              │
         │  │  (Manual Control)   │◄───────────────────┼──────┐       │
         │  └─────────────────────┘                    │      │       │
         │           │                 │               │      │       │
         │           │ BTN_1_MOD2      │ BTN_2_MOD2    │      │       │
         │           ↓                 ↓               │      │       │
         │  ┌─────────────────────┐  ┌─────────────────────┐  │       │
         │  │STATE_MANUAL_FLASH_YEL│ │STATE_MANUAL_FLASH_RED│ │       │
         │  │  (Flash Yellow)     │  │  (Flash Red)        │  │       │
         │  └─────────────────────┘  └─────────────────────┘  │       │
         │           │                         │              │       │
         │           │ BTN_1_MOD2              │ BTN_2_MOD2   │       │
         │           └─────────────────────────┴──────────────┘       │
         │                                                            │
         └────────────────────────────────────────────────────────────┘
```

---

## Traffic Light Phases (AUTO_NORM State)

When in `STATE_AUTO_NORM` with balanced durations (R = Y + G), the system cycles through 4 phases:

```plain
Phase 1: PHASE_NS_GREEN_EW_RED
┌─────────────────────────────┐
│  NS Direction: GREEN        │ Duration: greenDuration
│  EW Direction: RED          │ Duration: redDuration
└─────────────────────────────┘
               ↓ (when NS countdown = 0)
Phase 2: PHASE_NS_YELLOW_EW_RED
┌─────────────────────────────┐
│  NS Direction: YELLOW       │ Duration: yellowDuration
│  EW Direction: RED          │ Duration: yellowDuration
└─────────────────────────────┘
               ↓ (when countdown = 0)
Phase 3: PHASE_NS_RED_EW_GREEN
┌─────────────────────────────┐
│  NS Direction: RED          │ Duration: redDuration
│  EW Direction: GREEN        │ Duration: greenDuration
└─────────────────────────────┘
               ↓ (when EW countdown = 0)
Phase 4: PHASE_NS_RED_EW_YELLOW
┌─────────────────────────────┐
│  NS Direction: RED          │ Duration: yellowDuration
│  EW Direction: YELLOW       │ Duration: yellowDuration
└─────────────────────────────┘
               ↓ (when countdown = 0)
               └──────► Back to Phase 1
```

---

## Button Controls Summary

### BUTTON_1_MOD1 (Mode Switch)

- **Function**: Toggle between AUTO and MANUAL modes
- **From AUTO states** → Goes to `STATE_MANUAL`
- **From MANUAL states** → Goes to `STATE_AUTO_NORM`

### BUTTON_2_MOD1 (Sub-function in Current Mode)

- **In AUTO_NORM**: Enter `STATE_AUTO_RED`
- **In AUTO_RED**: Enter `STATE_AUTO_YEL`
- **In AUTO_YEL**: Enter `STATE_AUTO_GRN`
- **In AUTO_GRN**: Return to `STATE_AUTO_NORM`
- **In MANUAL**: Toggle manual sub-state (NS↔EW)

### BUTTON_1_MOD2 (Modifier 1)

- **In AUTO_RED**: Increment RED duration (+1)
- **In AUTO_YEL**: Increment YELLOW duration (+1)
- **In AUTO_GRN**: Increment GREEN duration (+1)
- **In MANUAL**: Enter `STATE_MANUAL_FLASH_YEL`
- **In MANUAL_FLASH_YEL**: Return to `STATE_MANUAL`

### BUTTON_2_MOD2 (Modifier 2)

- **In AUTO_RED**: Decrement RED duration (-1)
- **In AUTO_YEL**: Decrement YELLOW duration (-1)
- **In AUTO_GRN**: Decrement GREEN duration (-1)
- **In MANUAL**: Enter `STATE_MANUAL_FLASH_RED`
- **In MANUAL_FLASH_RED**: Return to `STATE_MANUAL`

---

## Duration Constraint

**Balance Condition**: `redDuration = yellowDuration + greenDuration`

- **If balanced**: Normal operation proceeds with countdown timers
- **If unbalanced**: System shows error and flashes YELLOW in AUTO_NORM

**Valid Range**: 1-99 seconds for each duration

---

## Key Features

### 1. **Automatic Mode (AUTO)**

- Normal traffic light cycling with countdown timers
- Configuration modes for each light color
- Balance validation with error indication
- Visual feedback through flashing lights during configuration

### 2. **Manual Mode (MANUAL)**

- Direct control of traffic light states
- Two sub-states for different direction priorities
- Flash modes for emergency/maintenance situations
- Immediate response without timers

### 3. **Safety Features**

- Balance checking prevents invalid timing configurations
- Flash yellow indication for error states
- All transitions are button-controlled (no automatic transitions between modes)
- Configuration persists across power cycles (saved to flash)

### 4. **Display System**

- 16x2 LCD display with real-time information
- Shows current state, countdown timers, and configuration values
- Distinct messages for each state
- Error messages for unbalanced configurations

---

## FSM Function Calls

### Core Functions

- **`fsm_init()`**: Initialize FSM to STATE_INIT
- **`fsm_run()`**: Main FSM execution (handles INIT→AUTO transition)
- **`fsm_button_scan()`**: Process button inputs and trigger state transitions
- **`fsm_countdown_update()`**: Update countdown timers (called every 1 second)
- **`fsm_lcd_update()`**: Refresh LCD display based on current state
- **`fsm_flash_update()`**: Control flashing lights (called every 500ms)

### Timing Requirements

- **Button scanning**: Continuous (scheduled task)
- **Countdown update**: Every 1000ms
- **Flash update**: Every 500ms
- **LCD update**: On state change or countdown update

---

## Implementation Notes

1. **State Persistence**: Duration values are saved to flash memory for retention across resets
2. **Debouncing**: Button inputs are debounced in the button module
3. **Thread Safety**: FSM uses a flag-based approach for LCD updates to avoid conflicts
4. **Modularity**: Separate modules for buttons, lights, LCD, timers, and FSM logic
5. **Scheduler**: Uses a cooperative scheduler for periodic tasks

---

## Error Handling

### Unbalanced Configuration

- **Condition**: When R ≠ Y + G
- **Behavior**:
  - Display "ERR: UNBALANCED" message
  - Flash YELLOW on all lights
  - No countdown operation
  - User must adjust durations in config modes

### Recovery

- Enter configuration mode (`BUTTON_2_MOD1` from AUTO_NORM)
- Adjust durations to satisfy balance equation
- Return to AUTO_NORM to resume normal operation

---
