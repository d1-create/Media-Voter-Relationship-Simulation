# MediaBiasSimulation

An agent-based, discrete-time simulation in modern C++ modeling ideological polarization, echo chambers, and commercial media radicalization.

## 📌 Table of Contents
* [1. Project Overview](#1-project-overview)
* [2. How it Works](#2-how-it-works)
    * [Stage One](#stage-one)
    * [Stage Two](#stage-two)
    * [Stage Three](#stage-three)
* [3. Important Notes](#notes-important)
* [4. Speed And Efficiency](#Data-Driven-vs-OOP)

## 1. Project Overview

The project synthesizes a basic version of two formal models from computational methods:
1. **The Deffuant–Weisbuch Bounded Confidence Model (2000):** Individual citizens filter ideological input through a strict cognitive tolerance threshold ($\epsilon$), updating personal belief states via linear convergence only when a signal falls within their acceptance window.
2. **The Hotelling–Downs Spatial Competition Model (1957):** Competing media firms dynamically adjust ideological coordinates ($B_j$) across a continuous 1D interval $[-1.0, 1.0]$ based on revenue optimization, oscillating between median-voter capture and fringe-monopoly extraction.

## 2. How it Works

Each discrete simulation turn represents one epoch ($t = \text{day}$).

### Stage One

**Data Collection**: Companies collect basic data on consumers from the (assumed) same source of information
*State Media* focuses audiences to the middle/centrist values on the political system, then *Privately-Owned* media tries to adapt to market changes to maximise profit or survive. 

### Stage Two

After the biases and polarisation calculations are complete, the media outlets then change their positions on the political spectrums gradually. There is a state-based business model of survival, stability and greed (profit maximisation).

### Stage Three

Data is exported to a file via graphs. One can visualise changes incrementally (every 20 days) on a terminal

## Notes (Important)

The program relies strictly on the standard C++ runtime library with zero external dependencies.
**This program is not designed for full-scale simulation. It is merely an enjoyable simulation and project which simulates a basic dynamic between voters and media in a world that desires polarisation and with minimal state intervention.**

## Data-Driven vs OOP

Data Driven approaches are far faster due to cache efficiency (storing in a sequential order in memory). It may be implemented in the future. The current model is a Object Oriented Model, which is generally easier to read/write c++ in.Another idea for enhanced performance would also be to implemented Parallel Processing and optimising the way voters and outlets find each other.
