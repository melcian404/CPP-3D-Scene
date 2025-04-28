# 3D Rendered Scene Using Visual Studio and C++

<video width="560" height="315" controls loop="" muted = "" autoplay="">
  <source src="https://github.com/melcian404/melcian404.github.io/raw/refs/heads/main/docs/assets/3Dvid.mp4">
</video>




<details>
	<summary>Project Descritpion</summary>
This artifact utilizes an OpenGL graphics processing unit (GPU) and C++ to create a 3D model using texture, lighting, and object rendering. It also controls the virtual environment with hot keys and camera positions. This includes zooming in and out, panning up and down, left and right, and tracking mouse movement. The scene required a combination of simple and complex objects. It depicts a scene of children's toys on a table and was initially created in July 2024 then revamped in April 2025.
</details>

<details>
	<summary>
		Why OpenGL?
	</summary>
	
Knowledge of OpenGL and 3D rendering is a significant advantage in today’s world, where 3D and Augmented Reality, Virtual Reality, and Mixed Reality are becoming an integral part of everyone’s daily life. This skill set has a demand to match. Immersive technology is one of the fastest-growing and most fascinating technologies available today. Many companies invest in this technology to enhance their products and appeal to end users. Meta has developed a mixed-reality headset that lets you see your surroundings and seamlessly integrate graphics, making them more realistic. Google is utilizing 3D imaging with Project Starline to enhance remote meetings and replicate an in-person meeting experience. Niantic is creating a cool augmented reality map of the world, where developers hide digital prizes near monumental structures. These are just a tiny example of what companies use 3D rendering for, so you can see that the potential and need for this skill set are limitless.	

</details>  

<details>
	<summary>
		Design Process
	</summary>

 - The design process began by adding the shapes and positioning them in the desired area of the scene. The keyboard and mouse hotkey movements were added to pan through the scene, ensuring objects were placed and sized correctly. Color hex codes were added to objects that were not wrapped in a texture. Texture wraps were added to the library and applied to objects. Shader materials like glass or wood were added to replicate the texture appearances. Lights were added to enhance the shader materials and create reflections from the objects. This project relied heavily on iterating shape meshes. The basic build of each shape was incorporated into the starter code, and I had to reuse the shape meshes multiple times to obtain all the necessary objects for this design. For example, the pyramid in this scene comprises three pyramids of different colors, blended to achieve the desired effect of multiple-colored faces.
  
-  When it comes to designing software, the best route is simplicity. It’s best to have a clear vision of the result and break it down into smaller, easy-to-handle portions to keep it organized and maintained. Code designed with detailed, easy-to-comprehend inline comments while keeping it in its simplest form is imperative for making code portable, usable, and fixable. This code was designed to be portable, allowing it to be transferred and reused in multiple methods with minimal changes. Integrating testing was performed numerous times as each method was created before advancing to the next task. This consistent checking was crucial to the success of the project.
  
</details>

<details>
  <summary>Example Object</summary>

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

***
