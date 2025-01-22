# OnlineInfoOlympiad
This project is a client-server application developed in C on Linux, simulating an online programming olympiad. Participants connect to a central server, receive programming problems, submit solutions, and get real-time feedback. The server handles solution evaluation, scoring, ensuring a seamless competitive environment. Using TCP/IP sockets for communication.

# Key Features
1. Client-Server Communication:
  - Implemented using TCP/IP sockets to facilitate reliable data transmission.
  - Clients interact with the server to receive problems, submit solutions, and view results.
2. Problem Management:
  - The server stores and distributes a predefined set of programming problems to participants.
  - Problems include metadata such as input/output formats, constraints, and example test cases.
3. Solution Submission and Evaluation:
  - Participants submit their solutions.
  - Automatic grading is performed by comparing the program's output against predefined test cases.
4. Real-Time Leaderboard:
  - Scores are computed based on the number of correct solutions.
5. Concurrency and Scalability:
  - The server handles multiple clients concurrently.
  - Efficient resource management ensures smooth operation under high load.

# Technical Details
1. Programming Language:
  - C with standard libraries and Linux-specific system calls for network and file handling.
2. Networking:
  - Sockets API used for communication between clients and the server.
3. File Handling:
  - The server manages solution submissions and logs results in structured directories.
4. Security:
  - Input validation and error handling to ensure robustness.
5. Platform:
  - Developed and tested on Linux (Ubuntu).
  - Utilizes Linux-specific tools like gcc, gdb, and system calls for process and thread management.

# Use Case
This project can be extended to host real-world online programming contests, training sessions, or educational platforms for teaching algorithms and competitive programming skills. It can also be integrated with web interfaces for a more user-friendly experience.

