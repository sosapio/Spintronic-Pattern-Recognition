import plotly.express as px
import pandas as pd

# Define the tasks for the research project with phases
df = pd.DataFrame([
    #dict(Task="Literature Review", Start='2024-08-25', Finish='2024-09-05', Phase="Plan"),
    dict(Task="Literature Review", Start='2024-08-25', Finish='2024-09-22', Phase="Execute"),
    #dict(Task="Literature Review", Start='2024-09-15', Finish='2024-09-22', Phase="Test"),
    dict(Task="Agree on Simulation Software", Start='2024-09-01', Finish='2024-09-05', Phase="Plan"),
    dict(Task="Agree on Simulation Software", Start='2024-09-05', Finish='2024-09-15', Phase="Execute"),
    dict(Task="Design Custom Circuit (FeFET)", Start='2024-09-11', Finish='2024-09-18', Phase="Plan"),
    dict(Task="Design Custom Circuit (FeFET)", Start='2024-09-15', Finish='2024-10-05', Phase="Execute"),
    dict(Task="Design Custom Circuit (FeFET)", Start='2024-10-05', Finish='2024-10-12', Phase="Test"),
    dict(Task="Design Custom Circuit (MRAM)", Start='2024-09-11', Finish='2024-09-18', Phase="Plan"),
    dict(Task="Design Custom Circuit (MRAM)", Start='2024-09-15', Finish='2024-10-05', Phase="Execute"),
    dict(Task="Design Custom Circuit (MRAM)", Start='2024-10-05', Finish='2024-10-12', Phase="Test"),
    dict(Task="Design Custom Circuit (Skyrmion-based)", Start='2024-09-11', Finish='2024-09-20', Phase="Plan"),
    dict(Task="Design Custom Circuit (Skyrmion-based)", Start='2024-09-20', Finish='2024-10-05', Phase="Execute"),
    dict(Task="Design Custom Circuit (Skyrmion-based)", Start='2024-10-05', Finish='2024-10-12', Phase="Test"),
    dict(Task="Implement Neural Network Architecture", Start='2024-09-15', Finish='2024-09-23', Phase="Plan"),
    dict(Task="Implement Neural Network Architecture", Start='2024-09-23', Finish='2024-10-05', Phase="Execute"),
    dict(Task="Implement Neural Network Architecture", Start='2024-10-05', Finish='2024-10-12', Phase="Test"),
    dict(Task="Integrate Neural Networks and Simulated Circuits", Start='2024-10-11', Finish='2024-10-15', Phase="Plan"),
    dict(Task="Integrate Neural Networks and Simulated Circuits", Start='2024-10-15', Finish='2024-10-25', Phase="Execute"),
    dict(Task="Integrate Neural Networks and Simulated Circuits", Start='2024-10-25', Finish='2024-10-30', Phase="Test"),
    dict(Task="Train and Optimize the Neural Network with Spintronic Synapses", Start='2024-10-26', Finish='2024-10-30', Phase="Plan"),
    dict(Task="Train and Optimize the Neural Network with Spintronic Synapses", Start='2024-10-30', Finish='2024-11-04', Phase="Execute"),
    dict(Task="Train and Optimize the Neural Network with Spintronic Synapses", Start='2024-11-04', Finish='2024-11-12', Phase="Test"),
    dict(Task="Analysis and Optimization of Results", Start='2024-11-10', Finish='2024-11-14', Phase="Plan"),
    dict(Task="Analysis and Optimization of Results", Start='2024-11-14', Finish='2024-11-19', Phase="Execute"),
    dict(Task="Analysis and Optimization of Results", Start='2024-11-19', Finish='2024-11-25', Phase="Test"),
    dict(Task="Prepare Final Documentation", Start='2024-11-25', Finish='2024-11-30', Phase="Plan"),
    dict(Task="Prepare Final Documentation", Start='2024-11-30', Finish='2024-12-10', Phase="Execute"),
])

# Create the Gantt chart
fig = px.timeline(df, x_start="Start", x_end="Finish", y="Task", color="Phase", title="Research Project on Neuromorphic Computing")
fig.update_yaxes(autorange="reversed")
fig.update_layout(
    xaxis_title="Timeline",
    yaxis_title="Tasks",
    legend_title="Phase",
    title_font_size=20,
    title_x=0.5,
    font=dict(size=26)  # Increase the font size for the entire chart
)

# Add vertical lines for milestones
milestones = [
    dict(date='2024-10-18', label="Homecoming"),
    dict(date='2024-11-25', label="Thanksgiving"),
    dict(date='2024-12-12', label="Final Report Due")
]

current_stage = [
    dict(date='2024-11-10', label="Weekly Project Report")
]

for milestone in milestones:
    fig.add_vline(x=milestone['date'], line=dict(color="black", width=1))
    fig.add_annotation(x=milestone['date'], y=1, text=milestone['label'], showarrow=False, yshift=10, font=dict(size=10))  # Increase the font size for annotations

for milestone in current_stage:
    fig.add_vline(x=milestone['date'], line=dict(color="red", width=2, dash="dash"))
    fig.add_annotation(x=milestone['date'], y=1, text=milestone['label'], showarrow=False, yshift=10, font=dict(size=16))  # Increase the font size for annotations

fig.show()