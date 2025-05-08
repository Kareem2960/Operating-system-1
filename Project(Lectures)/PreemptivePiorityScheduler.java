/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 */


package kggg.preemptivepiorityscheduler;

/**
 *
 * @author Kareem8
 */
import java.util.*;



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

public class PreemptivePiorityScheduler {
    public static void main(String[] args) {
        // Example processes
        Process[] processes = {
            new Process(1, 0, 10, 3),
            new Process(2, 1, 1, 1),
            new Process(3, 2, 2, 4),
            new Process(4, 3, 3, 2)
        };

        List<String> gantt = new ArrayList<>();
        int time = 0, completed = 0, n = processes.length;
        Process current = null;

        while (completed < n) {
            // Find highest priority process that has arrived and is not finished
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

        // Print Gantt chart
        System.out.print("Gantt Chart: ");
        for (String s : gantt) System.out.print(s + " ");
        System.out.println();

        // Print stats
        double avgTurn = 0, avgWait = 0, avgResp = 0;
        for (Process p : processes) {
            avgTurn += p.turnaround;
            avgWait += p.waiting;
            avgResp += p.response;
            System.out.printf("P%d: Turnaround=%d, Waiting=%d, Response=%d\n",
                p.id, p.turnaround, p.waiting, p.response);
        }
        System.out.printf("Average Turnaround Time: %.2f\n", avgTurn/n);
        System.out.printf("Average Waiting Time: %.2f\n", avgWait/n);
        System.out.printf("Average Response Time: %.2f\n", avgResp/n);
    }
}

