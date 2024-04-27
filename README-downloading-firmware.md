

First, make sure the project has these files otherwise STM32CubeIDE won't have the correct build and debug settings (you won't even be able to build):
- .cproject
- .mxproject
- .project
- .ioc file
![](attachments/12a23c9a285e1b0db0b344e08e6fe9de.png)

Then import the project as  `File > Import > Projects from Folder or Archive`


![](attachments/a4ed56e054d821a229f5d3cb17cb25a0.png)
![](attachments/1f590cb8b3262f73dec52e8a5b6e9160.png)

Select the project root and make sure the following dialogue has the checkbox checked in the form field for the project and click `Finish` to import the project

![](attachments/0b93b173b6ad62440f674de0c17ec51b.png)


> [!NOTE] If this checkbox is not checked it won't import the project. It could be that the CubeIDE thinks you already have imported the project into your workspace, so you need to delete the project from your workspace (or open a new workspace), and refresh/clean the workspace, then you should be able to import again.


You should now see the project in the sidebar as e.g. `38-04-usb-sleuth (in src)`

Click on the project and then see if you can build it:
![](attachments/e55aec405d73f69e787cfa3a0cbacf1f.png)

Note: Build issues could be missing driver dependencies or other source/header files missing, or a bad configuration of the Includes paths. HAL drivers should be in `src/Drivers`.

To flash the firmware right click on the project and go to `Run As > STM32 C/C++ Application`

![](attachments/5ce56866261687c275a8fa65fda2c581.png)

Make sure the console shows the correct project is being build and run. It is easy to accidentally select the wrong project. 

![](attachments/a50988ff8e46a63d073c160608a2389b.png)

Verify download is complete - you should now have a flashed device!
![](attachments/715d29fe3adcf9974e866354933c5e9b.png)
