#include "Laborator5.h"
#include "Transform3D.h"
#include <vector>
#include <string>
#include <iostream>
#include <math.h>
#include <Core/Engine.h>

using namespace std;

Laborator5::Laborator5()
{
}

Laborator5::~Laborator5()
{
}

void Laborator5::Init()
{
	renderCameraTarget = false;

	camera = new Laborator::Camera();
	minimapCamera = new Laborator::Camera();

	camera->Set(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	minimapCamera->Set(glm::vec3(0, 30.0f, 0), glm::vec3(0, 1.0f, 0), glm::vec3(0, 0, 1.0f));

	//PLAYER-UL
	{
		Mesh* playerMesh = new Mesh("player");
		playerMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "man.obj");
		meshes[playerMesh->GetMeshID()] = playerMesh;
	}


	//INAMICII -> LUPI
	{
		Mesh* enemyMesh = new Mesh("enemy");
		enemyMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "wolf.obj");
		meshes[enemyMesh->GetMeshID()] = enemyMesh;
	}

	//TURNURILE
	{
		Mesh* towerMesh = new Mesh("tower");
		towerMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "tower.obj");
		meshes[towerMesh->GetMeshID()] = towerMesh;
	}

	//SFERA - folosita pentru proiectile si pentru vietile player-ului
	{
		Mesh* sphereMesh = new Mesh("sphere");
		sphereMesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[sphereMesh->GetMeshID()] = sphereMesh;
	}

	//PENTRU MINIMAP
	{
		Mesh* meshMinimap = new Mesh("box");
		meshMinimap->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[meshMinimap->GetMeshID()] = meshMinimap;
	}

	//PENTRU TURNURI
	for (int i = 0; i < 3; i++) {
		Tower tower;
		tower.angle = 45.0f;
		towersVector.push_back(tower);
	}
	//coordonatele turnurilor
	
	//cel din stanga
	towersVector[0].x = -5.0f;

	//cel din dreapta
	towersVector[1].x = 5;
	towersVector[1].z = 0;

	//cel din fata
	towersVector[2].z = -5;

//	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);
	fov = 60;
	var = 10.0f;
	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
	minimapMatrix = glm::ortho(15.5f, -15.5f, 10.0f, -10.0f, 0.05f, 100.0f);
}

void Laborator5::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator5::PistolShooting() {
	
	for (int i = 0; i < pistolVector.size(); i++) {
		pistolVector[i].x += pistolVector[i].speed * cos(pistolVector[i].angle);
		pistolVector[i].z += pistolVector[i].speed * sin(pistolVector[i].angle);
		pistolVector[i].distancePistol -= 2;
		if (pistolVector[i].distancePistol == 0) {
			pistolVector.erase(pistolVector.begin() + i);
		}
	}

}

void Laborator5::RifleShooting() {

	for (int i = 0; i < rifleVector.size(); i++) {
		rifleVector[i].x += rifleVector[i].speed * cos(rifleVector[i].angle);
		rifleVector[i].z += rifleVector[i].speed * sin(rifleVector[i].angle);
		rifleVector[i].distanceRifle -= 2;
		rifleVector[i].scaleRifle = 0.1f;
		if (rifleVector[i].distanceRifle == 0) {
			rifleVector.erase(rifleVector.begin() + i);
		}
	}
}

void Laborator5::TowerShooting() {

	float angle = 0;
	for (int i = 0; i < towersVector.size(); i ++) {
		for (int j = 0; j < wolvesVector.size(); j ++) {
			if (wolvesVector[j].isOnDiagonal) {
				angle = atan2f(wolvesVector[j].x - towersVector[i].x, wolvesVector[j].z - towersVector[i].z);
				towersVector[i].angle = angle;
				for (int k = 0; k < towersVector[i].towerBulletsVector.size(); k++) {
					
					//daca e turnul din dreapta
					if (towersVector[i].x == 5) {
						towersVector[i].towerBulletsVector[k].angle = RADIANS(90 - angle);
					}
					//daca e turnul din mijloc
					else if (towersVector[i].x == 0) {
						towersVector[i].towerBulletsVector[k].angle = RADIANS(angle);
					}
					//daca e turnul din stanga
					else if (towersVector[i].x == -5) {
						towersVector[i].towerBulletsVector[k].angle = RADIANS(angle);
					}
					towersVector[i].towerBulletsVector[k].x += cos(towersVector[i].towerBulletsVector[k].angle) * towersVector[i].towerBulletsVector[k].speed;
					towersVector[i].towerBulletsVector[k].z -= sin(towersVector[i].towerBulletsVector[k].angle) * towersVector[i].towerBulletsVector[k].speed;
				
					if (towersVector[i].towerBulletsVector[k].x < 5 && towersVector[i].towerBulletsVector[k].z < 5) {
						glm::mat4 modelMatrix = glm::mat4(1);
						modelMatrix = glm::translate(modelMatrix, glm::vec3(towersVector[i].towerBulletsVector[k].x, towersVector[i].towerBulletsVector[k].y, 
							towersVector[i].towerBulletsVector[k].z));
						modelMatrix = glm::scale(modelMatrix, glm::vec3(towersVector[i].towerBulletsVector[k].scaleTowerBullets));
						RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
					}
				}
			}
		}
	}
}

void Laborator5::GrenadeShooting() {


	for (int i = 0; i < grenadeVector.size(); i++) {
		grenadeVector[i].x += grenadeVector[i].speed * cos(grenadeVector[i].angle);
		grenadeVector[i].z += grenadeVector[i].speed * sin(grenadeVector[i].angle);
		grenadeVector[i].distanceGrenade -= 2;

		//daca e mai mica decat jumatate din distanta, inseamna ca a trecut de jumatate -> scad y
		if (grenadeVector[i].distanceGrenade < 130) {
			grenadeVector[i].y -= 0.019;
		}
		else if (grenadeVector[i].distanceGrenade < 140 && grenadeVector[i].distanceGrenade > 120) {
			grenadeVector[i].y += 0.019;
		}
		//altfel, daca e mai mare decat jumatate din distanta, inseamna ca e pana in jumatate -> cresc y
		else if (grenadeVector[i].distanceGrenade > 140) {
			grenadeVector[i].y += 0.019;
		}
		if (grenadeVector[i].distanceGrenade == 0) {
			grenadeVector.erase(grenadeVector.begin() + i);
		}
	}
}

void Laborator5::PistolWolvesCollision() {
	
	float first, second;
	for (int i = 0; i < wolvesVector.size();) {
		bool deletedWolf = false;
		for (int j = 0; j < pistolVector.size();) {
			first = pow(wolvesVector[i].x - pistolVector[j].x, 2);
			second = pow(wolvesVector[i].z - pistolVector[j].z, 2);
			collisionDistPistol = sqrt(first + second);

			if (collisionDistPistol <= deltaDistance) {
				wolvesVector[i].lifes -= 2;
				pistolVector.erase(pistolVector.begin() + j);
			}
			//nu se sterg proiectilele decat daca distanta de coliziune > 1.5
			else if (collisionDistPistol > deltaDistance) {
				j++;
			}
		}
		if (deletedWolf == false) {
			i++;
		}
	}
}

void Laborator5::RifleWolvesCollision() {

	float first, second;
	for (int i = 0; i < wolvesVector.size();) {
		bool deletedWolf = false;
		for (int j = 0; j < rifleVector.size();) {
			first = pow(wolvesVector[i].x - rifleVector[j].x, 2);
			second = pow(wolvesVector[i].z - rifleVector[j].z, 2);
			collisionDistRifle = sqrt(first + second);

			if (collisionDistRifle <= deltaDistance) {
				wolvesVector[i].lifes --;
				rifleVector.erase(rifleVector.begin() + j);
			}
			//nu se sterg proiectilele doar daca distanta de coliziune > 1.5
			else if (collisionDistRifle > deltaDistance) {
				j++;
			}
		}
		if (deletedWolf == false)
			i++;
	}
}

void Laborator5::GrenadeWolvesCollision() {
	
	float first, second;
	for (int i = 0; i < wolvesVector.size();) {
		bool deletedWolf = false;
		for (int j = 0; j < grenadeVector.size();) {
			first = pow(wolvesVector[i].x - grenadeVector[j].x, 2);
			second = pow(wolvesVector[i].z - grenadeVector[j].z, 2);
			collisionDistGrenade = sqrt(first + second);

			if (collisionDistGrenade <= deltaDistance) {
				wolvesVector[i].lifes -= 3;
				grenadeVector.erase(grenadeVector.begin() + j);
			}
			//nu se sterg proiectilele doar daca distanta de coliziune > 1.5
			else if (collisionDistPistol > deltaDistance) {
				j++;
			}
		}
		if (deletedWolf == false)
			i++;
	}
}

void Laborator5::TowersWolvesCollision() {

	float first, second;
	for (int k = 0; k < towersVector.size(); k++) {
		for (int i = 0; i < wolvesVector.size();) {
			bool deletedWolf = false;
			for (int j = 0; j < towersVector[k].towerBulletsVector.size();) {
				first = pow(wolvesVector[i].x - towersVector[k].towerBulletsVector[j].x, 2);
				second = pow(wolvesVector[i].z - towersVector[k].towerBulletsVector[j].z, 2);
				collisionDistTower = sqrt(first + second);

				if (collisionDistTower <= deltaDistanceTower) {
					wolvesVector[i].lifes -= 5;
					towersVector[k].towerBulletsVector.erase(towersVector[k].towerBulletsVector.begin() + j);
				}
				//nu se sterg proiectilele doar daca distanta de coliziune > 1.5
				else if (collisionDistTower > deltaDistanceTower) {
					j++;
				}
			}
			if (deletedWolf == false)
				i++;
		}
	}
}

float auxX = 0;
float auxY = 0;
void Laborator5::Update(float deltaTimeSeconds)
{
	
	//pentru rotatia vietilor jucatorului
	angularStep += deltaTimeSeconds;

	//PISTOLUL
	if (player.typeShooting == 1) {
		fov = 60;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
		player.scalePlayer = 0.15f;
		perspective = 1;
		PistolShooting();
		PistolWolvesCollision();
	}
	else if (player.typeShooting == 2) {
		fov = 20;
		player.scalePlayer = 0.04f;
		perspective = 2;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
		RifleShooting();
		RifleWolvesCollision();
	}
	else if (player.typeShooting == 3) {
		fov = 60;
		player.scalePlayer = 0.15f;
		perspective = 3;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
		GrenadeShooting();
		GrenadeWolvesCollision();
	}
	//INAMICII

	//verificare conditii pentru inamici

	for (int i = 0; i < wolvesVector.size(); i++) {
		//pe prima orizontala
		if (wolvesVector[i].x <= 9 && wolvesVector[i].z == -9) {
			wolvesVector[i].x += 0.022;
			wolvesVector[i].isOnDiagonal = false;
		}
		//pe ultima orizontala
		else if ((wolvesVector[i].x <= 9 && (int)wolvesVector[i].z == 9)) {
			wolvesVector[i].x += 0.022;
			wolvesVector[i].angle = M_PI / 2;
			wolvesVector[i].isOnDiagonal = false;
		}
		// pe diagonala
		else {
			wolvesVector[i].x -= 0.022;
			wolvesVector[i].z += 0.022;
			wolvesVector[i].angle = -M_PI / 4;
			wolvesVector[i].isOnDiagonal = true;
		}
		if (wolvesVector[i].x > 8.5f && wolvesVector[i].z > 8.5f) {
			//inamicii dispar si player-ul pierde o viata
			wolvesVector.erase(wolvesVector.begin() + i);
			player.lifes--;
		}
	}

	if (player.lifes == 0) {
		player.angleWhenDead -= 0.025f;
		player.gameOver = true;
		if (player.angleWhenDead <= -M_PI / 2) {
			player.disappear = true;
		}
	}

	//PENTRU ANIMATIA LUPILOR
	for (int i = 0; i < wolvesVector.size(); i++) {
		if (wolvesVector[i].lifes <= 0) {
			wolvesVector[i].angleWhenDead -= 0.025f;
			if (wolvesVector[i].angleWhenDead <= -M_PI / 2) {
				wolvesVector[i].disappear = true;
				wolvesVector.erase(wolvesVector.begin() + i);
			}
		}
	}

	double time = Engine::GetElapsedTime();

	//dupa 5 secunde, mai adauga in vector inca un inamic

	if (time > auxTime) {
		Wolf wolf;
		wolf.x = -9;
		wolf.y = 0;
		wolf.z = -9;
		wolf.angle = M_PI / 2;
		
		//daca au aparut cel mult 10 inamici, distanta de timp dintre ei va fi de 5 secunde
		if (time < nextTime) {
			auxTime += 5;
		}
		//altfel, daca deja s-a ajuns la al 11-lea inamic, distanta de timp dintre ei va fi de 3 secunde
		else if (time > nextTime) {
			auxTime += 3;
			wolf.lifes *= 2;
		}
		wolvesVector.push_back(wolf);
	}

	TowerShooting();
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
	DrawScene();

	glm::ivec2 resolution2 = window->GetResolution();
	glViewport(900, 20, resolution2.x / 4, resolution2.y / 4);

	for (int i = 0; i < towersVector.size(); i ++) {
		if (Engine::GetElapsedTime() > towersVector[i].time) {
			TowerBullet towerBullet;
			towerBullet.x = towersVector[i].x;
			towerBullet.y = towersVector[i].y;
			towerBullet.z = towersVector[i].z;
			towerBullet.scaleTowerBullets = 0.1f;
			towerBullet.angle = towersVector[i].angle;
			towerBullet.speed = 0.005f;

			towerBullet.y += 0.5;
			towersVector[i].towerBulletsVector.push_back(towerBullet);
			towersVector[i].time += 4;
		}
	}

	TowerShooting();
	for (int i = 0; i < towersVector.size(); i++) {
		TowersWolvesCollision();
	}
	DrawMinimap();
	
}

void Laborator5::DrawMinimap() {

	//TURNURILE
	for (int i = 0; i < towersVector.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(towersVector[i].x, towersVector[i].y, towersVector[i].z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f));
		RenderMeshMinimap(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}

	//PLAYER-UL
	if (!player.disappear) {
		if (player.lifes > 0) {

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(player.scaleMinimapPlayer));
			modelMatrix = glm::rotate(modelMatrix, (float)M_PI, glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, player.angle, glm::vec3(0, 1, 0));
			RenderMeshMinimap(meshes["box"], shaders["Color"], modelMatrix);
		}
		else {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(player.scaleMinimapPlayer));
			modelMatrix = glm::rotate(modelMatrix, player.angleWhenDead, glm::vec3(1, 0, 0));
			RenderMeshMinimap(meshes["box"], shaders["Color"], modelMatrix);
		}
	}

	//LUPII
	for (int i = 0; i < wolvesVector.size(); i++) {

		if (!wolvesVector[i].disappear) {
			//daca inca mai au vieti, ii desenam normal
			if (wolvesVector[i].lifes > 0) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(wolvesVector[i].x, wolvesVector[i].y,
					wolvesVector[i].z));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f));
				modelMatrix = glm::rotate(modelMatrix, wolvesVector[i].angle, glm::vec3(0, 1, 0));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 0, 1));
				RenderMeshMinimap(meshes["box"], shaders["VertexNormal"], modelMatrix);
			}

			else {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(wolvesVector[i].x, wolvesVector[i].y, wolvesVector[i].z));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f));
				modelMatrix = glm::rotate(modelMatrix, wolvesVector[i].angleWhenDead, glm::vec3(0, 0, 1));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 0, 1));
				RenderMeshMinimap(meshes["box"], shaders["VertexNormal"], modelMatrix);
			}
		}
	}
}

void Laborator5::DrawScene() {

	if (!player.disappear) {
		if (player.lifes > 0) {

			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(player.scalePlayer));
			modelMatrix = glm::rotate(modelMatrix, (float)M_PI, glm::vec3(0, 1, 0));
			modelMatrix = glm::rotate(modelMatrix, player.angle, glm::vec3(0, 1, 0));
			RenderMesh(meshes["player"], shaders["VertexNormal"], modelMatrix);
		}
		else {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(player.scalePlayer));
			modelMatrix = glm::rotate(modelMatrix, player.angleWhenDead, glm::vec3(1, 0, 0));
			RenderMesh(meshes["player"], shaders["VertexNormal"], modelMatrix);
		}
	}

	//VIETILE JUCATORULUI

	for (int i = 0; i < player.lifes; i++) {

		if (i == 0) {
			//prima viata
			{
				if (perspective == 2) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y + 0.1f, camera->GetTargetPosition().z);
					modelMatrix *= Transform3D::RotateOZ(angularStep);
					modelMatrix *= Transform3D::Translate(0.15, 0.05, 0);
					modelMatrix *= Transform3D::Scale(0.05f, 0.05f, 0.05f);
					RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
				}
				else if (perspective == 1 || perspective == 3) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y + 0.45f, camera->GetTargetPosition().z);
					modelMatrix *= Transform3D::RotateOZ(angularStep);
					modelMatrix *= Transform3D::Translate(0.25, 0.35, 0);
					modelMatrix *= Transform3D::Scale(0.15f, 0.15f, 0.15f);
					RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
				}
			}
		}
		else if (i == 1) {
			//a doua viata
			{
				if (perspective == 2) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y + 0.1f, camera->GetTargetPosition().z);
					modelMatrix *= Transform3D::RotateOY(angularStep);
					modelMatrix *= Transform3D::Translate(0.15, 0, 0.05);
					modelMatrix *= Transform3D::Scale(0.05f, 0.05f, 0.05f);
					RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
				}
				else if (perspective == 1 || perspective == 3) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y + 0.45f, camera->GetTargetPosition().z);
					modelMatrix *= Transform3D::RotateOY(angularStep);
					modelMatrix *= Transform3D::Translate(0.25, 0, 0.25);
					modelMatrix *= Transform3D::Scale(0.15f, 0.15f, 0.15f);
					RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
				}
				
			}
		}
		else if (i == 2) {
			//a treia viata
			{
				if (perspective == 2) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y + 0.1f, camera->GetTargetPosition().z);
					modelMatrix *= Transform3D::RotateOX(angularStep);
					modelMatrix *= Transform3D::Translate(0, 0.25, 0.05);
					modelMatrix *= Transform3D::Scale(0.05f, 0.05f, 0.05f);
					RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
				}
				else if (perspective == 1 || perspective == 3) {
					glm::mat4 modelMatrix = glm::mat4(1);
					modelMatrix *= Transform3D::Translate(camera->GetTargetPosition().x, camera->GetTargetPosition().y + 0.45f, camera->GetTargetPosition().z);
					modelMatrix *= Transform3D::RotateOX(angularStep);
					modelMatrix *= Transform3D::Translate(0, 0.35, 0.35);
					modelMatrix *= Transform3D::Scale(0.15f, 0.15f, 0.15f);
					RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
				}
			}
		}
	}

	//TURNURILE
	for (int i = 0; i < towersVector.size(); i++) {
		
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(towersVector[i].x, towersVector[i].y, towersVector[i].z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.005f));
		modelMatrix = glm::rotate(modelMatrix, towersVector[i].angle, glm::vec3(0, 1, 0));
		RenderMesh(meshes["tower"], shaders["VertexNormal"], modelMatrix);
	}

	//GLOANTELE DE LA PISTOL
	if (!player.gameOver) {
		for (int i = 0; i < pistolVector.size(); i++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(pistolVector[i].x, pistolVector[i].y,
				pistolVector[i].z));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(pistolVector[i].scalePistol));
			modelMatrix = glm::rotate(modelMatrix, pistolVector[i].angle, glm::vec3(0, 0, 1));
			RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
		}
	}
	//GLOANTELE DE LA PUSCA
	if(!player.gameOver) {
		for (int i = 0; i < rifleVector.size(); i++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(rifleVector[i].x, rifleVector[i].y,
				rifleVector[i].z));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(rifleVector[i].scaleRifle));
			modelMatrix = glm::rotate(modelMatrix, rifleVector[i].angle, glm::vec3(0, 0, 1));
			RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
		}
	}

	//GRENADELE
	if(!player.gameOver) {
		for (int i = 0; i < grenadeVector.size(); i++) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(grenadeVector[i].x, grenadeVector[i].y,
				grenadeVector[i].z));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(grenadeVector[i].scaleGrenade));
			modelMatrix = glm::rotate(modelMatrix, grenadeVector[i].angle, glm::vec3(0, 0, 1));
			RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
		}
	}

	//LUPII
	for (int i = 0; i < wolvesVector.size(); i++) {
		
		if (!wolvesVector[i].disappear) {
			//daca inca mai au vieti, ii desenam normal
			if (wolvesVector[i].lifes > 0) {
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(wolvesVector[i].x, wolvesVector[i].y,
					wolvesVector[i].z));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
				modelMatrix = glm::rotate(modelMatrix, wolvesVector[i].angle, glm::vec3(0, 1, 0));
				RenderMesh(meshes["enemy"], shaders["VertexNormal"], modelMatrix);
			}
			
			else{
				glm::mat4 modelMatrix = glm::mat4(1);
				modelMatrix = glm::translate(modelMatrix, glm::vec3(wolvesVector[i].x, wolvesVector[i].y, wolvesVector[i].z));
				modelMatrix = glm::scale(modelMatrix, glm::vec3(1.5f));
				modelMatrix = glm::rotate(modelMatrix, wolvesVector[i].angleWhenDead, glm::vec3(0, 0, 1));
				RenderMesh(meshes["enemy"], shaders["VertexNormal"], modelMatrix);
			}
		}
	}
}

void Laborator5::FrameEnd()
{
	DrawCoordinatSystem(minimapCamera->GetViewMatrix(), minimapMatrix);
}

void Laborator5::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

void Laborator5::RenderMeshMinimap(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix) {

	if (!mesh || !shader || !shader->program)
		return;

	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, false, glm::value_ptr(minimapCamera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(minimapMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

float fy = RADIANS(60);
float M = -5.0f;
float N = 5;
void Laborator5::OnInputUpdate(float deltaTime, int mods)
{
	//tipuri de arme in functie de ce tasta este apasata
	if (window->KeyHold(GLFW_KEY_1)) {
		player.typeShooting = 1;
	}

	if (window->KeyHold(GLFW_KEY_2)) {
		player.typeShooting = 2;
	}

	if (window->KeyHold(GLFW_KEY_3)) {
		player.typeShooting = 3;
	}

	// move the camera only if MOUSE_RIGHT button is pressed
	
	float cameraSpeed = 2.0f;

	if (window->KeyHold(GLFW_KEY_W)) {
		camera->MoveForward(cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_A)) {
		// TODO : translate the camera to the left
		camera->TranslateRight(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_S)) {
		// TODO : translate the camera backwards
		camera->MoveForward(-cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_D)) {
		// TODO : translate the camera to the right
		camera->TranslateRight(cameraSpeed * deltaTime);
	}

	if (window->KeyHold(GLFW_KEY_P)) {
		fy += 0.2f;
		projectionMatrix = glm::perspective(fy, window->props.aspectRatio, 0.1f, 250.0f);
	}

	if (window->KeyHold(GLFW_KEY_L)) {
		var += deltaTime * 5;
		projectionMatrix = glm::ortho(-var, var, -var, var, 0.01f, 50.0f);
	}

	if (window->KeyHold(GLFW_KEY_K)) {
		fov += deltaTime * 100;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
	}

	if (window->KeyHold(GLFW_KEY_J)) {
		var -= deltaTime * 5;
		projectionMatrix = glm::ortho(-var, var, -var, var, 0.01f, 50.0f);
	}

	if (window->KeyHold(GLFW_KEY_H)) {
		fov -= deltaTime * 100;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
	}

	if (window->KeyHold(GLFW_KEY_M)) {
		// TODO
		M += 0.1f;
		N -= 0.1f;
		projectionMatrix = glm::ortho(-15.0f, 15.0f, M, N, 0.1f, 250.0f);
	}
}

void Laborator5::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
	}
}

void Laborator5::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = true;
			camera->RotateThirdPerson_OX(-deltaY * sensivityOX);
			camera->RotateThirdPerson_OY(-deltaX * sensivityOY);
			player.angle -= sensivityOY * deltaX;					//cu cat rotesc camera
		}
	}
}

void Laborator5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
		
		if (player.typeShooting == 1) {
			//proiectile pe care le trage player-ul
			Pistol pistolBullets;
			pistolBullets.angle = -player.angle - M_PI / 2;
			pistolBullets.x = camera->GetTargetPosition().x;
			pistolBullets.y = 0.5f;
			pistolBullets.z = camera->GetTargetPosition().z;
			pistolVector.push_back(pistolBullets);
		}

		else if (player.typeShooting == 2) {

			Rifle rifleBullets;
			rifleBullets.angle = -player.angle - M_PI / 2;
			rifleBullets.x = camera->GetTargetPosition().x;
			rifleBullets.y = 0.15f;
			rifleBullets.z = camera->GetTargetPosition().z;
			rifleVector.push_back(rifleBullets);
		}

		else if (player.typeShooting == 3) {
			
			Grenade grenadeBullets;
			grenadeBullets.angle = -player.angle - M_PI / 2;
			grenadeBullets.x = camera->GetTargetPosition().x;
			grenadeBullets.y = 0.2f;
			grenadeBullets.z = camera->GetTargetPosition().z;
			grenadeVector.push_back(grenadeBullets);
		}
	}
}

void Laborator5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator5::OnWindowResize(int width, int height)
{
}
