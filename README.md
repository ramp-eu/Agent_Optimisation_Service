# Agent Optimization

This project is part of [Better Fatory](https://betterfactory.eu/). For more information check the RAMP Catalogue entry for the [components](https://github.com/ramp-eu).

## Contents
-   [Background](#Background)
-   [Install](#Install)
-   [Usage](#Usage)
-   [Contribution](#contribution)
-   [License](#license)

## Background

The objective of the Agent Optimization package is to compute the optimal number of agnets (AGVs, humans, ...) for material transport.

The package uses the following input information stored in MongoDB:
- Distances matrix – distance from station to station (NxN matrix, N - number of stations).
- Flow rate matrix – representing material flow rate from station to station (NxN matrix, N - number of stations).
- Agent parameters (agent speed, agent capacity, agent availability, time to load, time to unload, traffic factor, operator efficiency).

The input data can be inserted for processing:
- Manually using MondoDB commands.
- Using your own applikaction.
- Using Material Flow package which extract data from Real-Time Locating System (RTLS) package.

## Install

The installation instructions are meant for devices with
- Ubuntu 20.04 or higher

First, update and restart your system:
```
sudo apt update
sudo apt-get upgrade
sudo reboot
```

There are 2 ways of deploying Agent Optimization
- Using GitHub repo to construct the docker image
- Download the Agent Opimization docker image

### Download GitHub repo and build the docker container
Follow the [Install Docker Engine on Ubuntu](https://docs.docker.com/engine/install/ubuntu/):
```
sudo apt-get remove docker docker-engine docker.io containerd runc
sudo apt-get update
sudo apt-get install \
    ca-certificates \
    curl \
    gnupg \
    lsb-release
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo docker run hello-world
```

Then [Docker Compose on Linux systems](https://docs.docker.com/compose/install/)
```
sudo curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
sudo chmod +x /usr/local/bin/docker-compose
docker-compose --version
```

Install [Git](https://github.com/git-guides/install-git)
```
sudo apt install git
```

Create a directory (GIT_REPO as example), init the git and clone it
```
mkdir GIT_REPO
cd Desktop/GIT_REPO/
git init
git clone https://github.com/BF-OPILdev/AgentOptimization
cd 
```

Go to the docker directory where the dockerfile is located and compose the image
```
sudo chmod 666 /var/run/docker.sock
docker build -t agent-optimization .
```

The docker image is ready to use. You can follow the [Usage](#Usage) chapter.

### Download Agent Optimization docker image

Follow the [Install Docker Engine on Ubuntu](https://docs.docker.com/engine/install/ubuntu/):
```
sudo apt-get remove docker docker-engine docker.io containerd runc
sudo apt-get update
sudo apt-get install \
    ca-certificates \
    curl \
    gnupg \
    lsb-release
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /usr/share/keyrings/docker-archive-keyring.gpg
echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/docker-archive-keyring.gpg] https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
sudo docker run hello-world
```

Download the docker image
```
docker pull docker.ramp.eu/opil/AgentOptimization
```

## Usage

Agent Optimization can be used as stand alone package. It contains MongoDB. To start the Agent Optimization docker image use
```
sudo docker run -d -p 27017:27017 --name ao-instance1 -v mongo-data:/data/db  agent-optimization
```

If the container is not running find it's ID and restat it (cc221ab99ad58dbd881579a51a8664692548a20f85d11b08934d10432aaf9194 is an example)
```
docker ps -a --no-trunc
docker restart cc221ab99ad58dbd881579a51a8664692548a20f85d11b08934d10432aaf9194
```

To interact with docker image use
```
sudo docker exec -it ao-instance1 /bin/bash
```

Now the container is running and we can insert data to MongoDB manually
```
mongo
db.createCollection("AGENT_OPTIMIZATION")
use AGENT_OPTIMIZATION
db.createCollection("AGENTS")
db.AGENTS.insertOne({"request_id" : "333", "N" : "3", "distance_matrix" : "0,50,0;0,0,60;50,0,0;", "flowrate_matrix" : "0,10,0;0,0,15;-1,0,0;", "agent_speed" : "50", "agent_load_time" : "0.75", "agent_unload_time" : "0.5", "agent_capacity" : "2", "agent_availability" : "0.95", "traffic_factor" : "0.9", "operator_efficiency" : "1.0", "result_optimal_number_of_agents" : "1.3694", "result_optimal_number_of_agents_rounded" : "2", "result_DONE" : "0" })
db.AGENTS.find()
exit
exit
```
- The first command starts the MongoDB shell. 
- The next 3 commands will create the default database / documents. The documents are stored in AGENT_OPTIMIZATION/AGENTS.
- A new record is inserted into MongoDB.
- Display the inserted record (to verify if the record was saved in database).
- Exit from MongoDB (to shell).
- Second exit exits from docker image (to host OS).

The record consists of:
- request_id - unique ID which is used to identify the record,
- N - size of distance and flow rate matrices,
- distance_matrix and flowrate_matrix - matrices. Rows are separated by ; and numbers in rows by ,
- agent_speed - agent speed [m/min],
- agent_load_time - time to load [min],
- agent_unload_time - time to unload [min],
- agent_capacity - agent capacity [pcs],
- agent_availability - agent availability,
- traffic_factor - traffic factor,
- operator_efficiency - operator efficiency
- result_optimal_number_of_agents - optimal number of agents (the result written by the application)
- result_optimal_number_of_agents_rounded - rounded value of optimal number of agents (the result written by the application)
- result_DONE - 0 - the optimization was not performed, 1 - the result were written back to database

To start the optimization from the host OS:
```
docker exec -it ao-instance1 /code/main
```
- Executes the /code/main program inside the Docker image.

To start the optimization inside the Docker image:
```
sudo docker exec -it ao-instance1 /bin/bash
/code/main
```
- Starts the bash (command line) inside Docker.
- Executes the optimization locally in Docker.

If the request ID and results (ideal cycle time, number of agents, rounded value) are displayed, the data from MongoDB was processed correctly. Otherwise, the structure of the MongoDB document is wrong, or the document is missing, etc.
