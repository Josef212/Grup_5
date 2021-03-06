#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

// Reference at https://youtu.be/6OlenbCC4WI?t=382

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	graphics = NULL;
	fx = 0;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	App->player->exploding = false;

	graphics = App->textures->Load("rtype/intro.png");
	App->audio->PlayMusic("rtype/intro.ogg", 0.0f);
	fx = App->audio->LoadFx("rtype/starting.wav");
	App->renderer->camera.x = App->renderer->camera.y = 0;

	fade_out = false;

	App->player->lifes = 3;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	App->textures->Unload(graphics);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// Draw everything --------------------------------------	
	App->renderer->Blit(graphics, 0, -15, NULL);

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP && fade_out == false)
	{
		App->audio->PlayFx(fx);
		App->fade->FadeToBlack(this, App->scene_space, 3.0f);
		fade_out = true;
	}

	return UPDATE_CONTINUE;
}