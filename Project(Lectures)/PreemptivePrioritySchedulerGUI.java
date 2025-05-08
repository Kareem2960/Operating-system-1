import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.util.ArrayList;
import java.util.List;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Locale;

class Process {
    int id, arrival, burst, priority;
    int remaining, start = -1, finish, waiting, turnaround, response;

    Process(int id, int arrival, int burst, int priority) {
        this.id = id;
        this.arrival = arrival;
        this.burst = burst;
        this.priority = priority;
        this.remaining = burst;
    }
}

public class PreemptivePrioritySchedulerGUI extends JFrame {
    private JTable processTable;
    private JTable resultTable;
    private JTextArea ganttChartArea;
    private DefaultTableModel processModel;
    private DefaultTableModel resultModel;
    private JButton simulateButton;
    private JButton addProcessButton;
    private JButton clearButton;

    public PreemptivePrioritySchedulerGUI() {
        setTitle("Preemptive Priority Scheduler");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(1000, 800);
        setLayout(new BorderLayout());

        // Create input panel
        JPanel inputPanel = new JPanel(new BorderLayout());
        
        // Process table
        String[] columns = {"Process ID", "Arrival Time", "Burst Time", "Priority"};
        processModel = new DefaultTableModel(columns, 0) {
            @Override
            public boolean isCellEditable(int row, int column) {
                return column > 0; // Only allow editing of arrival, burst, and priority
            }
        };
        processTable = new JTable(processModel);
        JScrollPane processScrollPane = new JScrollPane(processTable);
        inputPanel.add(processScrollPane, BorderLayout.CENTER);

        // Buttons panel
        JPanel buttonPanel = new JPanel();
        addProcessButton = new JButton("Add Process");
        simulateButton = new JButton("Simulate");
        clearButton = new JButton("Clear");
        
        buttonPanel.add(addProcessButton);
        buttonPanel.add(simulateButton);
        buttonPanel.add(clearButton);
        inputPanel.add(buttonPanel, BorderLayout.SOUTH);

        // Create output panel
        JPanel outputPanel = new JPanel(new BorderLayout());
        
        // Gantt Chart
        ganttChartArea = new JTextArea(3, 40);
        ganttChartArea.setEditable(false);
        JScrollPane ganttScrollPane = new JScrollPane(ganttChartArea);
        outputPanel.add(ganttScrollPane, BorderLayout.NORTH);

        // Results table
        String[] resultColumns = {"Process ID", "Turnaround Time", "Waiting Time", "Response Time"};
        resultModel = new DefaultTableModel(resultColumns, 0) {
            @Override
            public boolean isCellEditable(int row, int column) {
                return false; // Make results table read-only
            }
        };
        resultTable = new JTable(resultModel);
        JScrollPane resultScrollPane = new JScrollPane(resultTable);
        outputPanel.add(resultScrollPane, BorderLayout.CENTER);

        // Add panels to frame
        add(inputPanel, BorderLayout.NORTH);
        add(outputPanel, BorderLayout.CENTER);

        // Add action listeners
        addProcessButton.addActionListener(e -> addProcess());
        simulateButton.addActionListener(e -> simulate());
        clearButton.addActionListener(e -> clearAll());

        // Add some sample processes
        addSampleProcesses();
    }

    private void addSampleProcesses() {
        processModel.addRow(new Object[]{1, 0, 10, 3});
        processModel.addRow(new Object[]{2, 1, 1, 1});
        processModel.addRow(new Object[]{3, 2, 2, 4});
        processModel.addRow(new Object[]{4, 3, 3, 2});
    }

    private void addProcess() {
        int newId = processModel.getRowCount() + 1;
        processModel.addRow(new Object[]{newId, 0, 0, 0});
    }

    private void clearAll() {
        processModel.setRowCount(0);
        resultModel.setRowCount(0);
        ganttChartArea.setText("");
    }

    private void simulate() {
        try {
            // Get processes from table
            int n = processModel.getRowCount();
            if (n == 0) {
                JOptionPane.showMessageDialog(this, "Please add at least one process.");
                return;
            }

            Process[] processes = new Process[n];
            for (int i = 0; i < n; i++) {
                int id = (int) processModel.getValueAt(i, 0);
                int arrival = Integer.parseInt(processModel.getValueAt(i, 1).toString());
                int burst = Integer.parseInt(processModel.getValueAt(i, 2).toString());
                int priority = Integer.parseInt(processModel.getValueAt(i, 3).toString());
                processes[i] = new Process(id, arrival, burst, priority);
            }

            // Run simulation
            List<String> gantt = new ArrayList<>();
            int time = 0, completed = 0;
            Process current = null;

            while (completed < n) {
                Process next = null;
                for (Process p : processes) {
                    if (p.arrival <= time && p.remaining > 0) {
                        if (next == null || p.priority < next.priority) {
                            next = p;
                        }
                    }
                }
                if (next == null) {
                    gantt.add("-");
                    time++;
                    continue;
                }
                if (next.start == -1) next.start = time;
                gantt.add("P" + next.id);
                next.remaining--;
                time++;
                if (next.remaining == 0) {
                    next.finish = time;
                    completed++;
                }
            }

            // Calculate times
            for (Process p : processes) {
                p.turnaround = p.finish - p.arrival;
                p.waiting = p.turnaround - p.burst;
                p.response = p.start - p.arrival;
            }

            // Update GUI
            updateResults(processes, gantt);
        } catch (NumberFormatException e) {
            JOptionPane.showMessageDialog(this, "Please enter valid numbers for arrival time, burst time, and priority.");
        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "An error occurred: " + e.getMessage());
        }
    }

    private void updateResults(Process[] processes, List<String> gantt) {
        // Clear previous results
        resultModel.setRowCount(0);
        ganttChartArea.setText("");

        // Update Gantt chart
        StringBuilder ganttText = new StringBuilder("Gantt Chart: ");
        for (String s : gantt) {
            ganttText.append(s).append(" ");
        }
        ganttChartArea.setText(ganttText.toString());

        // Update results table
        double avgTurn = 0, avgWait = 0, avgResp = 0;
        NumberFormat nf = NumberFormat.getNumberInstance(Locale.US);
        nf.setMinimumFractionDigits(2);
        nf.setMaximumFractionDigits(2);
        
        for (Process p : processes) {
            resultModel.addRow(new Object[]{
                p.id,
                nf.format(p.turnaround),
                nf.format(p.waiting),
                nf.format(p.response)
            });
            avgTurn += p.turnaround;
            avgWait += p.waiting;
            avgResp += p.response;
        }

        // Add averages
        resultModel.addRow(new Object[]{
            "Average",
            nf.format(avgTurn/processes.length),
            nf.format(avgWait/processes.length),
            nf.format(avgResp/processes.length)
        });
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            PreemptivePrioritySchedulerGUI gui = new PreemptivePrioritySchedulerGUI();
            gui.setVisible(true);
        });
    }
}