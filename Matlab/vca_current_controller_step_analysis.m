clearvars; close all; clc; clear workspace;
format long g;

%% Parameters
run("parameters.m");
filename = "data/current_control_step_log.csv";

%% Read data
data = readmatrix(filename);
if size(data,2) < 3
    error("Expected 5 columns: [time_us, current_amps, ].");
end
t_s  = data(:,1) / 10e6;
current_amps = data(:,2);
target_current_amps = data(:,3);

%% Analyze Data
[meets_constraints, results, fig] = ctrls.step_sequence_analysis( ...
    t_s, current_amps, target_current_amps, vcacc_constraints, ...
    "plot_title", "VCA Current Step Response Analysis", ...
    "x_axis_label", "Time (s)", ...
    "y_axis_label", "Current (A)");


struct2table(results)