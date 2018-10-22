# Workflow

Required. Define the tasks involved in the process and the dependencies between the tasks. A workflow consists of multiple steps, each of which can perform a specific task.

## workflow format

```
workflow:
  <task name>:
    description: <task info>
    tool: <toolName:version>
    resources：<resources required by the workflow>
    commands：<commands>
    commandsIter: <commands with variable>
    depends：< dependent task>
```

## Field

### workflow field

<table>
   <tr>
      <td>field</td>
      <td>required</td>
      <td>type</td>
      <td>description</td>
   </tr>
   <tr>
      <td>task name</td>
      <td>Yes</td>
      <td>String   </td>
      <td>The name of task. It must consist of lower case alphanumeric characters or '-', and must start and end with an alphanumeric character. And must be no more than 40 characters.</td>
   </tr>
   <tr>
      <td>description</td>
      <td>No</td>
      <td>String   </td>
      <td>Information about this task, must be no more than 255 characters.</td>
   </tr>
   <tr>
      <td>tool</td>
      <td>Yes</td>
      <td>String   </td>
      <td>The tool and version of the current task required, format "toolName:toolVersion". For example, the bwa of version 0.7.12 is set to "tool: bwa:0.7.12".</td>
   </tr>
   <tr>
      <td>resources</td>
      <td>No</td>
      <td>struct</td>
      <td>Compute Resources required by the task.</td>
   </tr>
   <tr>
      <td>commands</td>
      <td>No</td>
      <td>array[string]</td>
      <td>The command executed in the container. The length of the array indicates the number of concurrent. Each member represents a command executed in a container.In the following example, if there are four lines in the command, the number of concurrent containers is 4, and each container 
         executes a different command.

         commands:
         - sh /obs/shell/run-xxx/run.sh 1 a 
         - sh /obs/shell/run-xxx/run.sh 2 a 
         - sh /obs/shell/run-xxx/run.sh 1 b 
         - sh /obs/shell/run-xxx/run.sh 2 b 

Note 1: One of field commands and commandsIter must be set. Use commandsIter if the commands need to pass variables, otherwise use commands.
      </td>
   </tr>
   <tr>
      <td>commandsIter</td>
      <td>No</td>
      <td>Struct</td>
      <td>The command executed in the container, and the difference between command and commandIter is that the commandIter supports shell scripts with variables.</td>
   </tr>
   <tr>
      <td>depends</td>
      <td>No</td>
      <td>array[Struct]</td>
      <td>Specify other tasks that the current task depends on.</td>
   </tr>
</table>



### resource field description

<table>
   <tr>
      <td>field</td>
      <td>required</td>
      <td>type</td>
      <td>description</td>
   </tr>
   <tr>
      <td>memory</td>
      <td>No</td>
      <td>String</td>
      <td>
         The amount of memory resources required, in G. Format: "Number + Unit".
         <ul>
            <li>The number can be decimals.</li>
            <li>The unit is G or g.</li>
         </ul>
         For example, if the memory size required is 4G, you can fill in "4G" or "4g" here.
      </td>
   </tr>
   <tr>
      <td>cpu</td>
      <td>No</td>
      <td>String</td>
      <td>The amount of memory resources required, in C. Format: "Number + Unit".The number can be decimals.The unit is C or c.</td>
   </tr>
</table>


### commandsIter field description

<table>
   <tr>
      <td>field</td>
      <td>required</td>
      <td>type</td>
      <td>description</td>
   </tr>
   <tr>
      <td>command</td>
      <td>Yes</td>
      <td>String</td>
      <td>A shell script with variables, for example:

        echo ${1} ${2} ${item} 

There are two ways to define variables: 
<ul>
<li>
Variable parameters defined in commandsIter, including vars and varsIter parameters, you can set one of them. The format is “${n}”, n is a positive integer starting at 1. And it will be replaced by the nth element of vars. The vars needs to list all combinations of parameter, and the varsIter is an automatic traversal combination of parameter.
</li>
<li>Built-in variable "${item}". Represent the order number of all the possible parameter combinations.
</li>
</ul>
For example:

    commandsIter: 
      command: echo ${1} ${item} 
        vars:
          - a
          - b 
          - c 
   
Then the final command will be:

    - echo a 0 
    - echo b 1 
    - echo c 2
</td>
   </tr>
   <tr>
      <td>vars</td>
      <td>No</td>
      <td>array[array] </td>
      <td>A two-dimensional array, will be used to replace the command variable, represent all the possible parameter combinations. 
      <ul>
      <li>
      In the two-dimensional array, the members of each row represent the variables ${1}, ${2}, ${3}, etc. in the command. ${1} represents the first member of each line, ${2} represents the second member of each line, and ${3} represents the third member of each line.
      </li>
      <li>
      The length of the two-dimensional array indicates that how many times the command will be executed with different parameters. Each line of the array is used to instantiate the command.The number of rows in the array is the number of k8s job that will run.
      </li>
      </ul>
      For example, the vars has four lines.
      
        command: echo ${1} ${2} ${item} 
        vars: 
          - [0, 0] # 0 -> ${1}; 0 -> ${2}; 0 -> ${item}
          - [0, 1] # 0 -> ${1}; 1 -> ${2}; 1 -> ${item} 
          - [1, 0] # 1 -> ${1}; 0 -> ${2}; 2 -> ${item} 
          - [1, 1] # 1 -> ${1}; 1 -> ${2}; 3 -> ${item} 
        
4 k8s jobs will run to execute the commands.<br>
For the 1st job, the command is:

    echo 0 0 0 

For the 2nd job, the command is: 

    echo 0 1 1 
    
For the 3rd job, the command is:

    echo 1 0 2 
    
For the 4th job, the command is: 

    echo 1 1 3
    
</td>
   </tr>
   <tr>
      <td>varsIter</td>
      <td>No</td>
      <td>array[array]</td>
      <td>A two-dimensional array. VarsIter list all the possible parameters for every position in the command line. And we will use algorithm Of Full Permutation to generate all the permutation and combinations for these parameter that will be used to replace the ${n} variable.The first row member of the array replace the variable ${1} in the command, the second row member replace the variable ${2} in the command, and so on.
      
For example,
      
      commandsIter:
        command: sh /tmp/step1.splitfq.sh ${1} ${2} ${3}
        varsIter: - ["sample1", "sample2"]
                  - [0, 1]
                  - [25]
                  
then the finalcommand will be:

      sh /tmp/scripts/step1.splitfq.sh sample1 0 25 
      sh /tmp/scripts/step1.splitfq.sh sample2 0 25 
      sh /tmp/scripts/step1.splitfq.sh sample1 1 25 
      sh /tmp/scripts/step1.splitfq.sh sample2 1 25 

If there are many array members per line, you can use the range function.<br>The format of range function: range(start, end, step) Start and end are all integer. And step can only be positive integer. <br>
If you do not specify step, the default is 1.

Range(1, 4) represents array [1,2,3]

Range(1, 10, 2) represents array [1, 3, 5, 7, 9] 

    varsIter: - range(0, 4) 
the same as: 

    varsIter: - [0, 1, 2, 3]
</td>
   </tr>
</table>

### **Depends field description**

<table>
   <tr>
      <td>field</td>
      <td>required</td>
      <td>type</td>
      <td>description</td>
   </tr>
   <tr>
      <td>target</td>
      <td>Yes</td>
      <td>String</td>
      <td>The name of the task relying on, make sure that the specified task name must exist in the workflow.</td>
   </tr>
   <tr>
      <td>type</td>
      <td>No</td>
      <td>String</td>
      <td>Dependency type, the value can be:
      <ul>
      <li>
      whole, overall dependencies, this is the default.
      </li>
      <li>
      iterate, iterative dependency.
      </li>
      </ul>
      For example, if both task A and task B are executed concurrently 100.
    <ul>
    <li>
     Setting “whole” indicates that task B can start execution after all 100 steps of task A finished.
     </li>
     <li>
     Setting “iterate” means that the 1st step of task A is completed, then the 1st step of the task B can start execution. Iterative execution can improve the overall concurrency efficiency.
     </li>
     </ul>
     </td>
   </tr>
</table>













### Workflow example

```
workflow:
  job-a:
    tool: nginx:latest
    resources:
      memory: 2G
      cpu: 1c
    commands:
      - sleep `expr 3 \* ${wait-base}`; echo ${output-prefix}job-a | tee -a ${obs}/${output}/${result};
  job-b:
    tool: nginx:latest
    commandsIter:
      command: sleep `expr ${1} \* ${wait-base}`; echo ${output-prefix}job-b-${item} | tee -a ${obs}/${output}/${result};
      varsIter:
        - range(0, 3)
    depends:
      - target: job-a
        type: whole
  job-c:
    tool: nginx:latest
    type: GCS.Job
    resources:
      memory: 8G
      cpu: 2c
    commandsIter:
      command: sleep `expr ${1} \* ${wait-base}`; echo ${output-prefix}job-c-${item} | tee -a ${obs}/${output}/${result};
      varsIter:
        - [3, 20]
    depends:
      - target: job-a
        type: iterate
      - target: job-b
```
