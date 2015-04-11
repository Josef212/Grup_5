#include "Globals.h"
#include "Application.h"
#include "ModuleSceneStage1.h"

// Reference at https://www.youtube.com/watch?v=OEhmUuehGOA

ModuleSceneStage1::ModuleSceneStage1(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	background = NULL;
}

ModuleSceneStage1::~ModuleSceneStage1()
{}

// Load assets
bool ModuleSceneStage1::Start()
{
	LOG("Loading ken scene");
	
	background = App->textures->Load("lvl1.png");
	App->audio->PlayMusic("stage1.ogg", 1.0f);
	App->player->Enable();
	
	return true;
}

// UnLoad assets
bool ModuleSceneStage1::CleanUp()
{
	LOG("Unloading stage scene");

	App->textures->Unload(background);
	App->player->Disable();
	
	return true;
}

// Update: draw background
update_status ModuleSceneStage1::Update()
{

	float scroll_speed = 0.75f;

	if (App->renderer->camera.x >= (-3930 + SCREEN_WIDTH)*SCREEN_SIZE)
	{
		App->player->position.x += 1;
		App->renderer->camera.x -= 3;
	}
	else
		App->fade->FadeToBlack(this, App->scene_end, 2.0f);


	// Draw everything --------------------------------------
	App->renderer->Blit(background, 0, 0, NULL);

	return UPDATE_CONTINUE;
}