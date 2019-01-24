# Stanford CS248 Assignment 2: MeshEdit
The repository is located at https://github.com/stanford-cs248/triangle-mesh-editor. The wiki (https://github.com/stanford-cs248/triangle-mesh-editor/wiki) on that page will be the primary source of information about this assignment, this document only contains administrative details about grading and submission.

## Due Date
The assignment is due Feb. 7 at 11:59:59 PM.

## Code Environment
This codebase should compile on Linux, Mac OS X, and Windows on a typical environment. The build instructions given on [the project page](https://github.com/stanford-cs248/triangle-mesh-editor/wiki/Build-Instructions) will walk you through installing dependencies and building the code. If you have difficulties running the code on your local machine, the rice cluster machines (rice.stanford.edu) have all the packages required to build the project.

When you have successfully built your code, you should get an executable named `cardinal3d`. Upon starting the program, it should be in model mode. The [User Guide](https://github.com/stanford-cs248/triangle-mesh-editor/wiki/User-Guide-(MeshEdit)) details how to use the interface. For convenience, some keyboard shortcuts are reproduced below.

| Key                   | Command                                            |
| :-------------------: | :--------------------------------------------:     |
| `space`               | switch to navigate mode                            |
| `tab`                 | show/hide info panel                               |
| hold `control`        | temporarily switch to translation in edit mode     |
| hold `shift`          | temporarily switch to scaling in edit mode         |
| hold `alt/option`     | temporarily switch to rotation in edit mode        |
| `e`                   | cycle through **e**dit modes                       |
| `b`                   | toggle **b**evel mode                              |
| `n`                   | select **n**ext halfedge                           |
| `t`                   | select **t**win halfedge                           |
| `h`                   | select **h**alfedge of current element             |
| `T`                   | **T**riangulate mesh                               |
| `s`                   | **s**ubdivide Catmull-Clark                        |
| `S`                   | **S**ubdivide linear                               |
| `bksp/del`            | erase selected edge                                |
| `f`                   | **f**lip selected edge                             |
| `c`                   | **c**ollapse selected edge                         |
| `p`                   | s**p**lit selected edge triangle meshes only!      |
| `u`                   | **u**psample triangle meshes only!                 |
| `i`                   | **i**sotropic remesh triangle meshes only!         |
| `d`                   | **d**ownsample triangle meshes only!               |
| `w` then `0--9`       | **w**rite scene to numbered buffer                 |
| `l` then `0--9`       | **l**oad scene from numbered buffer                |

## Evaluation
For this assignment, you will implement methods in meshEdit.cpp.

The User Guide on the wiki describes a large number of features that are in principle available in MeshEdit mode. You do not have to implement all of these features to receive full credit on the assignment! However, you do have to successfully implement a subset of the features. Implementing additional features beyond the required subset will earn you extra credit points.

The particular requirements, functions that you have to implement, and the percentage of the grade they correspond to, are:

### 1. EdgeSplit and EdgeFlip (20%)

 - splitEdge()
 - flipEdge()
 
### 2. LinearSubdivision and 3. CatmullClarkSubdivision (20% each)

 - subdivideQuad()
 - computeLinearSubdivisionPositions()
 - computeCatmullClarkPositions()
 - assignSubdivisionIndices()
 - buildSubdivisionVertexList()
 - buildSubdivisionFaceList()
 
### 4. LoopSubdivision (20%)

 - upsample()
 
### 5. Create one beautiful 3D model using Cardinal3D (20%)

Every team is required to submit a 3D model created from cube.dae using their implementation of Cardinal3D, which will be automatically entered into a class-wide 3D modeling competition. Models will be critiqued and evaluated based on both technical sophistication and aesthetic beauty. Note: Use of any other 3D package (e.g., free or commercial 3D modelers like Maya or Blender) is strictly prohibited! This model must be created by opening cube.dae, applying the operations implemented as part of the assignment, and saving the result.
**You may have to implement at least one bevel operation (we found FaceBevel most useful) in extra credit to do this part!**

Include this model in the root directory of your submission as `model.dae`.

### Extra Credit

Here is a list of local operations:

 - VertexBevel
 - EdgeBevel
 - FaceBevel
 - EraseVertex
 - EraseEdge
 - EdgeCollapse
 - FaceCollapse
 - EdgeFlip
 - EdgeSplit

The global operations, and their dependency on local operations, are as follows:

 - Triangulation
 - LinearSubdivision
 - CatmullClarkSubdivision
 - LoopSubdivision - depends on EdgeSplit and EdgeFlip
 - IsotropicRemeshing - depends on EdgeSplit, EdgeFlip, and EdgeCollapse
 - Simplification - depends on EdgeCollapse
 
Each additional local operation beyond the requirements will be worth 2.5% of the assignment grade; each additional global operation will be worth 5% of the assignment grade. The maximum possible grade on the assignment is 120%. You need to specify clearly what you implemented in your writeup.

## Writeup
Additionally, you will submit a short document with your SuNET ID's, explaining what you have implemented (possibly including extra credit), and any particular details of your submission. If your submission includes any implementations which are not entirely functional, please detail what works and what doesn't, along with where you got stuck. This document does not need to be long; correctly implemented features may simply be listed, and incomplete features should be described in a few sentences at most.

The writeup must be a pdf, markdown, or plaintext file. Include it in the root directory of your submission as writeup.pdf, writeup.md, or writeup.txt.

Failure to submit this writeup will incur a penalty on the assignment.

## Submission Instructions
We are using [Canvas](https://canvas.stanford.edu) as our submission tool. You should create and upload a zipped folder of your entire `/src` subdirectory along with the writeup (e.g. writeup.txt) and 3D modeling submission (`model.dae`). Do not include your build subdirectory. You may work in teams of up to two people. If working in a team, please have only one person make the submission.

## Acknowledgement

CS248 course staff would like to thank Professor Keenan Crane and Carnegie Mellon University's course assistants for providing assignment materials.
