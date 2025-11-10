import pandas as pd
import os

# Define your motion files and labels
motions = {
    'idle.csv': 'idle',
    'left_right.csv': 'left_right',
    'up_down.csv': 'up_down',
    'walking.csv': 'walking'
}

all_data = []

for filename, label in motions.items():
    if os.path.exists(filename):
        with open(filename, 'r', errors='ignore') as f:
            lines = f.readlines()

        # --- Find the actual header line ---
        start_row = None
        for i, line in enumerate(lines):
            # Look for the lowercase version of your sensor header
            if "acc_x" in line and "gyro_x" in line:
                start_row = i
                break

        if start_row is not None:
            # Read CSV from that header line
            df = pd.read_csv(filename, skiprows=start_row)
            df['label'] = label
            all_data.append(df)
            print(f"✅ Loaded {filename} with {len(df)} samples.")
        else:
            print(f"⚠️ Could not find data header in {filename}. File skipped.")
    else:
        print(f"⚠️ File not found: {filename}")

# --- Combine all loaded data into one DataFrame ---
if all_data:
    combined_df = pd.concat(all_data, ignore_index=True)
    combined_df.to_csv('all_data.csv', index=False)
    print("\n💾 Saved combined file as all_data.csv")
    print("Columns:", combined_df.columns.tolist())
else:
    print("❌ No valid data loaded. Please check your CSVs.")
