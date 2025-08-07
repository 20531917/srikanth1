# Teamcenter Migration: Architecture Plan

## 1. Introduction

This document outlines the proposed system architecture for the new Teamcenter environment. It covers hardware, software, network configuration, and system layout for all environments (Development, Test/QA, Production).

## 2. Current State Architecture (Source)

*   **Application Version:** [e.g., Teamcenter 11.2]
*   **Database:** [e.g., Oracle 12c on Server X]
*   **Server Environments:**
    *   **Production:**
        *   Corporate Server: [Server Name, OS, CPU, RAM]
        *   Database Server: [Server Name, OS, CPU, RAM]
        *   File Server / Volumes: [Server Name, OS, Storage Size]
        *   Web Tier (TCSS): [Server Name(s), OS, CPU, RAM]
    *   **Test/QA:** [Details]
    *   **Development:** [Details]
*   **Network Diagram:**
    *   [Insert or link to a diagram showing how the current servers are connected]

## 3. Future State Architecture (Target)

*   **Application Version:** [e.g., Teamcenter 14.1]
*   **Database:** [e.g., Oracle 19c on Server Y]
*   **Server Environments:**
    *   **Production:**
        *   Corporate Server(s): [Server Name(s), OS, CPU, RAM, Load Balancing details]
        *   Database Server: [Server Name, OS, CPU, RAM, Failover/RAC details]
        *   File Server / Volumes (FMS): [FSC Server Names, OS, Cache size, Volume locations]
        *   Web Tier (TCSS): [Server Name(s), OS, CPU, RAM, Load Balancing details]
        *   Active Workspace Gateway: [Server Name(s), OS, CPU, RAM]
    *   **Test/QA:** [Details - should be a near-replica of Production]
    *   **Development:** [Details]
*   **Network Diagram:**
    *   [Insert or link to a diagram showing the new architecture, including firewalls, ports, and network zones]

## 4. Software Bill of Materials (BOM)

| Component                  | Version (Source) | Version (Target) | Notes                               |
| -------------------------- | ---------------- | ---------------- | ----------------------------------- |
| Teamcenter                 | 11.2             | 14.1             |                                     |
| Active Workspace           | 4.0              | 7.0              |                                     |
| Oracle Database            | 12c              | 19c              |                                     |
| Java JRE/JDK               | 8                | 11 / 17          | Check compatibility matrix          |
| Web Browser (client)       | Chrome X         | Chrome Y         |                                     |
| CAD Integrations           | NX 12, CATIA V5-6 | NX 2206, CATIA...|                                     |
| Other Customizations       | ...              | ...              | Plan for recompilation/re-deployment|

## 5. Data Migration Server

*   **Purpose:** A dedicated server for running the ETL scripts. This isolates the migration workload and prevents impact on source/target systems.
*   **Server Specs:** [Server Name, OS, CPU, RAM]
*   **Required Software:**
    *   **C++ Compiler:** Microsoft Visual Studio with the C++ toolchain to compile the custom ITK utilities.
    *   **Teamcenter Rich Client:** Required to provide the ITK libraries and runtime environment for the migration utilities.
    *   **Python:** Used for the transformation scripts.
    *   **Oracle Client:** For database connectivity if needed.
    *   **Git:** For version control of the migration scripts.
    *   Sufficient storage for extracted data, log files, and exported physical files.

*   **ETL Process:**
    *   The migration process will use a set of custom-built C++ ITK utilities (see `/itk_source`) for high-performance data extraction and loading.
    *   These utilities are orchestrated by the `.bat` scripts in the `/etl` directory.
    *   Data transformation is handled by a central Python script.

## 6. Assumptions and Risks

*   **Assumptions:**
    *   [e.g., Network connectivity between all servers is available on required ports.]
    *   [e.g., Hardware will be provisioned by date X.]
*   **Risks:**
    *   [e.g., Performance of the new environment does not meet expectations.]
    *   [e.g., Delays in hardware procurement.]
