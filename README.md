# 🔧 K-LUT Technology Mapper for Boolean Networks

This project implements a C++ program for performing **technology mapping** of a Boolean network into **K-input Look-Up Tables (K-LUTs)**. It was developed as part of the CS516000 **FPGA Architecture & CAD** course at **National Tsing Hua University**.

The objective is to partition the network into **K-feasible cones**, minimizing the number of required LUTs while respecting structural constraints of the input graph.

---

## 📘 Problem Overview

A Boolean network is represented as a **Directed Acyclic Graph (DAG)**. Nodes represent logic gates or primary inputs/outputs. The goal is to map this network into a collection of K-LUTs such that:
- Each LUT covers a subgraph rooted at some gate
- The number of inputs to the LUT is ≤ K
- The total number of LUTs is minimized

---

## 🧠 Features

- Parses a Boolean network from a custom input file format
- Assigns levels to nodes using **reverse BFS**
- Greedily generates K-feasible cones using **input expansion with backtracking**
- Avoids redundant LUT creation through **visited** and **merged** tracking
- Supports **re-processing prematurely merged nodes**
- Outputs results to a formatted text file

---

## 🗂️ Files

- `main.cpp` – Core implementation
- `Makefile` – Compilation rules
- `checker` – Grading/execution interface (binary provided)
- `109006255_report.pdf` – Detailed design explanation

---

## 📥 Input Format

An input file describes a Boolean network:

<NetworkName> <N> <I> <O>
<Primary Input ID 1>
<Primary Input ID I>
<Primary Output ID 1>
<Primary Output ID O>
<Gate ID> <Input ID 1> <Input ID 2>


Where:
- `N`: number of gates
- `I`: number of primary inputs
- `O`: number of primary outputs

---

## 📤 Output Format

Each line in the output file represents a single LUT:
<Output ID> <1st Input ID> <2nd Output ID> <Kth Input ID>


---

## ⚙️ How to Compile

To compile the project:


make
To clean the compiled binaries:

make clean

🚀 Usage

./mapper <input_file> <output_file> <K>
Example:

./mapper testcase1.txt result1.txt 4
This maps the circuit in testcase1.txt using 4-input LUTs and writes the result to result1.txt.

📊 Implementation Summary
Key components of the algorithm include:

Graph Construction: Parses the file into a DAG with primary inputs/outputs and gate interconnections.

Level Assignment: Performs reverse BFS to determine processing order.

LUT Generation: Expands inputs level-wise while tracking merged nodes and avoiding overflow.

Optimization: Reprocesses prematurely merged nodes as needed.

File Output: LUTs are saved in the required submission format.

For full documentation of each step and design decisions, see the included 109006255_report.pdf.

🧪 Example Use Case

./mapper network.in mapped.out 5
network.in: input network file

mapped.out: output mapping result

5: maps using 5-input LUTs

👤 Author
Rogelio Martínez (109006255)
National Tsing Hua University
Course: FPGA Architecture & CAD (CS516000)
