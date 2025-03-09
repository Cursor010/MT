# Turing Machine Simulator

<p align="center">
  <a href="#about">About</a> •
  <a href="#features">Features</a> •
  <a href="#installation">Installation</a> •
  <a href="#usage">Usage</a> •
  <a href="#structure">Code Structure</a>
</p>

## About
A Qt-based Turing Machine simulator with visual execution and editing capabilities. This implementation features:

- Infinite tape visualization
- State transition table editor
- Step-by-step execution control
- Speed adjustment (100-1500ms)
- Input validation and error handling

## Features
### Core Functionality
- **Tape Operations**
  - Scrollable infinite tape visualization
  - Current position highlighting
  - Automatic tape initialization with blank symbols (^)
  
- **State Management**
  - Dynamic state addition/removal (q0-qN)
  - Transition table with multiple commands per cell
  - Visual state highlighting during execution

- **Execution Control**
  - Start/Stop/Pause/Step operations
  - Adjustable execution speed
  - Emergency stop functionality
  - Auto-scroll to current position

### Validation System
- Alphabet consistency checks
- Stop command (!) verification
- Input string validation
- Transition command syntax checking

## Installation
### Requirements
- Qt 5.15+ framework
- C++17 compatible compiler

### Build Instructions
```bash
