## Overview
Pressed is a language for making HTML presentations. A pressed file is divided into three main sections:
* **Objects**: Reusable object definition, based in CSS properties. The custom types allowed are *Slide*, *Image* and *Textblock*. All objects -including slides- must be defined in this section.
* **Structure**: Determines the distribution of the elements in each slide, allowing positioning around an anchor. The content of textblocks and images is also set here, using *add-with* blocks. 
* **Animations**: Each slide can have an animation block (*start-then-end*), with the possibility of adding *repeat* to any block. Some of the animations are *appear*, *disappear* and *rotate*.

## Aclaraciones
Algunas cosas a tener en cuenta para esta segunda entrega del trabajo práctico:
* Algunos de los tests propuestos en la primera entrega fueron dejados de lado, ya que requerían de validaciones a desarrollar en el backend.
* Dependiendo de cuestiones varias de desarrollo, se evaluará agregar nuevas animaciones para la siguiente etapa del trabajo, además de animaciones entre diapositivas (*fade-into* y similar).

## Librerías utilizadas
* glib.h: Utilizada por su implementación de HashTables y arreglos. 

# Original README

[![✗](https://img.shields.io/badge/Release-v1.1.0-ffb600.svg?style=for-the-badge)](https://github.com/agustin-golmar/Flex-Bison-Compiler/releases)

[![✗](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml/badge.svg?branch=production)](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml)

# Flex-Bison-Compiler

A base compiler example, developed with Flex and Bison.

* [Environment](#environment)
* [CI/CD](#cicd)
* [Recommended Extensions](#recommended-extensions)
* Installation
  * [Docker](doc/readme/Docker.md)
  * [Microsoft Windows](doc/readme/Windows.md)
  * [Ubuntu](doc/readme/Ubuntu.md)

## Environment

Set the following environment variables to control and configure the behaviour of the application:

|Name|Default|Description|
|-|:-:|-|
|`LOG_IGNORED_LEXEMES`|`true`|When `true`, logs all of the ignored lexemes found with Flex at DEBUGGING level. To remove those logs from the console output set it to `false`.|
|`LOGGING_LEVEL`|`INFORMATION`|The minimum level to log in the console output. From lower to higher, the available levels are: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` and `CRITICAL`.|

## CI/CD

To trigger an automatic integration on every push or PR (_Pull Request_), you must activate _GitHub Actions_ in the _Settings_ tab. Use the following configuration:

|Key|Value|
|-|-|
|Actions permissions|Allow all actions and reusable workflows|
|Artifact and log retention|`30 days`|
|Fork pull request workflows from outside collaborators|Require approval for all outside collaborators|
|Workflow permissions|Read repository contents and packages permissions|
|Allow GitHub Actions to create and approve pull requests|`false`|

After integration is done, change the badges of this `README.md` to point to the new repository.

## Recommended Extensions

* [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
* [Yash](https://marketplace.visualstudio.com/items?itemName=daohong-emilio.yash)
