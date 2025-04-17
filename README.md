# 3D Rendered Scene Using Visual Studio and C++

## Original Project

<details>
	<summary>
		3D Scene Origin Photo
	</summary>
	
  ![OriginalScene](https://github.com/user-attachments/assets/d2a74f46-bb75-414a-826e-67e28c2a1e82)
	
</details>

<details>
	<summary>
		Best Practice Standards
	</summary>
	
When it comes to designing software, the best route is simplicity. Code designed with detailed, easy-to-comprehend inline comments while keeping it in its simplest form is imperative for making code portable, usable, and fixable. This was my first time working with OpenGL and rendering 3D scenes. I learned how to track mouse movements and keyboard inputs and then map them to the project for controlling the scene. I gained a better understanding of manipulating cameras, lights, and object sizes to get the desired results needed to match the original scene.

</details>

<details>
	<summary>
		Why OpenGL?
	</summary>
	
Having knowledge of OpenGL and 3D rendering is a significant advantage in today’s world, where 3D and Augmented Reality, Virtual Reality, and Mixed Reality are becoming an integral part of everyone’s daily life. This skill set has a demand to match. Immersive technology is one of the fastest-growing and most fascinating technologies available today. Many companies are investing in this technology to enhance their products and better appeal to end users. Meta has developed a mixed-reality headset that enables you to see your surroundings and seamlessly integrate graphics within them, making them more realistic. Google is utilizing 3D imaging with Project Starline to enhance remote meetings and replicate the experience of an in-person meeting. Niantic is creating a cool augmented reality map of the world, where developers hide digital prizes near monumental structures. These are just a tiny example of what companies are using 3D rendering for, so you can see that the potential and need for this skill set are limitless.	

</details>  

<details>
	<summary>
		Design Process
	</summary>

 - The design process began by adding the shapes and positioning them in the general desired area within the scene. Then, keyboard and mouse movements were added to get a better look at the objects and ensure they were in their correct positions and sizes. After, basic colors were added to objects that weren’t using a texture wrap. Texture wraps were then applied to the more complicated objects. Shader materials were then added to give objects a shinier appearance, such as a “glass” appearance. Lights were then added to the scene to enhance the shader materials and create reflections from the objects. This project relied heavily on iterating shape meshes. The basic build of each shape was incorporated into the starter code, and I had to reuse the shape meshes multiple times to obtain all the necessary objects for this design. For example, the pyramid in this scene is composed of three pyramids of different colors, blended to achieve the desired effect of multiple-colored faces.
  
- Much of this code was designed to be portable, allowing it to be transferred and reused in future projects with minimal changes to achieve the desired result. When undertaking any future projects, it’ll be best to have a clear vision of the result and to break it down into smaller, easy-to-handle portions to keep the design organized and attainable. This approach was adopted throughout the project's development, and the organization of the steps helped prevent the project from becoming overwhelming. Ensuring that all components functioned correctly before advancing to the next task was crucial to the project's success.
  
</details>



## Enhanced Project


## Project Enhancement Plan
- To enhance this project, I will clean up the prior code to make it more reader-friendly with inline comments and improved overall organization. I'll combine methods and use better naming practices for those methods. Then, I'll add more complex objects to this scene. This will require loading new textures and JPEG image files, creating pathways to them, adding the necessary shape meshes, creating a new shape instance, and rendering all of the above. The SceneManager.cpp, SceneManager.h, and image library will need to be revisited for these tasks.

## Enhanced Project Result
- I exceeded my original expectations for this enhancement plan. My original intention was to add another complex object to the scene. I added three complex objects of varying shapes, colors, sizes, and other characteristics. I also excelled in refining existing methods by combining objects, eliminating multiple-line redundancies, and refining my inline comments to make them more comprehensible. I also added block comments at the top of each page, annotating the changes and additions I’ve completed.

+++++++++++++++++++++++

<details>
	<summary>Project Descritpion</summary>
	This artifact utilizes an OpenGL graphics processing unit (GPU) and C++ to create a 3D model using texture, lighting, and object rendering and controls the virtual environment with hotkeys and camera positions. This includes zooming in and out, panning up and down, left and right, as well as tracking mouse movement. The scene required a combination of simple and complex objects. It depicts a scene of children's toys on a table and was initially created on July 20, 2024.
</details>

<details>
  <summary>Project Enhancements</summary>

  -  Refined inline comments to be concise and descriptive for detailing each function of code.
  -  Adding block comments to detail portions of code completed.
  -  Adding three complex objects of varying shapes, colors, sizes, and characteristics.
  -  Refined and combined object methods to reduce code redundancies.

</details>

<details>
  <summary>Created Object Example</summary>

  ```C++
	void SceneManager::RenderTrees(){

		// declare the variables for the transformations
		glm::vec3 scaleXYZ;
		float XrotationDegrees = 0.0f;
		float YrotationDegrees = 0.0f;
		float ZrotationDegrees = 0.0f;
		glm::vec3 positionXYZ;

	// LEFT TREE TOP
		// Set object scale
		scaleXYZ = glm::vec3(3.0f, 8.0f, 3.0f);

		// set the XYZ rotation for the mesh
		XrotationDegrees = 0.0f;
		YrotationDegrees = 0.0f;
		ZrotationDegrees = 0.0f;

		// set object position   L-R     U-D    F-B
		positionXYZ = glm::vec3(-15.5f, 2.53f, -9.5f);

		// set the transformations into memory to be used on the drawn meshes
		SetTransformations(
			scaleXYZ,
			XrotationDegrees,
			YrotationDegrees,
			ZrotationDegrees,
			positionXYZ);
	
		// set texture and material
		SetShaderTexture("Treetop");
		SetTextureUVScale(2.0, 4.0);
		SetShaderMaterial("wood");
	
		// draw mesh
		m_basicMeshes->DrawConeMesh();
```

</details>

<details>
	<summary>Original Result Image</summary>
	
![Original Scene](https://github.com/melcian404/melcian404.github.io/blob/main/docs/assets/OriginalScene.png)
	
</details>



<details>
  <summary>Enhanced Result Image</summary>

![EnhancedScene](https://github.com/melcian404/melcian404.github.io/blob/main/docs/assets/EnhancedScene.png)

</details>



***
