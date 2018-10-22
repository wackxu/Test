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
        
Note 1: One of field commands and commandsIter must be set. Use commandsIter if the commands need to pass variables, otherwise use commands.</td>
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
   <tr>
      <td></td>
   </tr>
</table>
