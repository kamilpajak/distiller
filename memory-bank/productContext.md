# Product Context: Distiller

## Why This Project Exists

The Distiller project exists to automate and optimize the distillation process, which traditionally requires constant manual monitoring and adjustment. By automating this process, we can achieve:

1. **Improved Consistency**: Manual distillation can lead to variations in the final product due to human error or inconsistent monitoring. Automation ensures that each batch follows the same precise parameters.

2. **Enhanced Efficiency**: Automated systems can optimize heating, cooling, and flow rates to maximize yield while minimizing energy consumption.

3. **Reduced Labor**: The system eliminates the need for constant human supervision, freeing up the operator to focus on other tasks.

4. **Better Fraction Separation**: Precise control of temperature and flow allows for more accurate separation of different fractions (foreshots, heads, hearts, tails), resulting in a higher quality final product.

## Problems It Solves

1. **Inconsistent Quality**: Manual distillation often results in batch-to-batch variations. The automated system ensures consistent quality by precisely controlling all parameters.

2. **Time-Intensive Monitoring**: Traditional distillation requires constant attention to monitor temperatures, adjust heating, and collect different fractions. The Distiller system handles these tasks automatically.

3. **Fraction Mixing**: Improper separation of fractions can lead to contamination of the desired product. The automated valve control ensures each fraction is collected separately.

4. **Temperature Control Challenges**: Maintaining optimal temperature profiles throughout the distillation process is difficult manually. The system uses multiple temperature sensors and PID control to maintain precise temperature profiles.

5. **Flow Rate Management**: Controlling the flow rate of distillate is crucial for quality but difficult to manage manually. The automated flow control system maintains optimal flow rates throughout the process.

## How It Should Work

The distillation process should proceed through the following phases automatically:

1. **Heating Phase**: The system heats the mash to the appropriate temperature (40°C) using maximum power.

2. **Stabilization Phase**: The system waits for temperature stabilization (difference between bottom and top less than 2°C) before proceeding.

3. **Collection Phases**: The system automatically collects different fractions:
   - Early Foreshots (first 200ml)
   - Late Foreshots (next 400ml)
   - Heads (next 900ml)
   - Hearts (next 5000ml or until temperature increase detected)
   - Early Tails (next 700ml)
   - Late Tails (next 600ml)

4. **Finalization Phase**: The system shuts down heating, continues cooling for 10 minutes, then closes all valves.

Throughout these phases, the system should:
- Display current status on the LCD
- Adjust heating power as needed
- Control flow rates using PID control
- Direct distillate to the appropriate collection container
- Monitor temperatures at multiple points
- Track the weight/volume of collected fractions

## User Experience Goals

1. **Minimal Intervention**: The user should only need to set up the initial conditions and start the process. The system should handle everything else automatically.

2. **Clear Information Display**: The LCD should provide clear, concise information about the current state, temperatures, flow rates, and collected volumes.

3. **Predictable Operation**: The system should operate consistently from batch to batch, following the same sequence of operations.

4. **Reliable Performance**: The system should operate reliably over extended periods without failures or unexpected behavior.

5. **Safe Operation**: The system should incorporate safety features to prevent hazardous conditions such as overheating or pressure buildup.

6. **Quality Results**: The ultimate goal is to produce high-quality distillate with proper separation of fractions, maximizing the yield of the desired product (hearts) while minimizing contamination from other fractions.
