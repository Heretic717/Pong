
#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define released(b) (!input->buttons[b].is_down && input->buttons[b].changed)

// Variables

float arena_half_size_x = 850, arena_half_size_y = 450; // arena size

// Player
float player_1_p, player_1_dp, player_2_p, player_2_dp; // position and velocity
float player_half_size_x = 25, player_half_size_y = 120; // player size
int player_1_score = 0, player_2_score = 0; // scoring

// ball
float ball_p_x, ball_p_y, ball_dp_x = 700, ball_dp_y; 
float ball_half_size = 10;

// Player
internal void
simulate_player(float *p, float *dp, float ddp, float dt) {
	ddp -= *dp * 7.f;

	*p = *p + *dp * dt + ddp * dt * dt * .5f;
	*dp = *dp + ddp * dt;

	if (*p + player_half_size_y > arena_half_size_y) {
		*p = arena_half_size_y - player_half_size_y;
		*dp *= -1;
	}
	if (*p - player_half_size_y < -arena_half_size_y) {
		*p = player_half_size_y - arena_half_size_y;
		*dp *= -1;
	}

	
}

// Collision detection
internal bool
aabb_vs_aabb(float p1x, float p1y, float hs1x, float hs1y,
	float p2x, float p2y, float hs2x, float hs2y) {

		return (p1x + hs1x > p2x - hs2x &&
			p1x - hs1x < p2x + hs2x &&
			p1y + hs1y > p2y - hs2y &&
			p1y + hs1y < p2y + hs2y);

}

// Menu functionality
enum Gamemode {
	GM_MENU,
	GM_GAMEPLAY,
};

Gamemode current_gamemode;
int hot_button;
bool enemy_is_ai;

internal void 
simulate_game(Input* input, float dt) {

	draw_rect(0, 0, arena_half_size_x, arena_half_size_y, 0xf9c95d); // draw play area
	draw_arena_borders(arena_half_size_x, arena_half_size_y, 0x000000); // draw background

	// Game
	if (current_gamemode == GM_GAMEPLAY) {

		draw_rect(0, 0, 2, arena_half_size_y, 0xffffff);

		float player_1_ddp = 0.f; // acceleration
		if (!enemy_is_ai) {
			if (is_down(BUTTON_UP)) player_1_ddp += 4000;
			if (is_down(BUTTON_DOWN)) player_1_ddp -= 4000;
		}
		else {
			// old "simple" AI
			//if (ball_p_y > (player_1_p + player_half_size_y * .4f)) player_1_ddp += 4000;
			//if (ball_p_y < (player_1_p - player_half_size_y * .4f)) player_1_ddp -= 4000;

			// new, better AI
			if (ball_dp_x < 0) {
				if (player_1_p < 0) player_1_ddp += 4000;
				if (player_1_p > 0) player_1_ddp -= 4000;
			}
			else {
				if (ball_p_y > 400 && ball_p_y < 445) {
					//if (ball_p_y > (player_1_p + player_half_size_y * .4f) && ball_dp_y > 0) player_1_ddp = (ball_p_y - player_1_p) * -1000;
					//if (ball_p_y > (player_1_p + player_half_size_y * .4f) && ball_dp_y < 0) player_1_ddp = (ball_p_y - player_1_p) * 1000;
					if (ball_p_y > player_1_p && ball_dp_y > 10) player_1_ddp = (ball_p_y - player_1_p) * ball_dp_x;
					if (ball_p_y > player_1_p && ball_dp_y < 0) player_1_ddp = (ball_p_y - player_1_p) * ball_dp_x;
				}
				else if (ball_p_y < -400 && ball_p_y > -445) {
					if (ball_p_y < (player_1_p - player_half_size_y * .4f) && ball_dp_y < 0) player_1_ddp = (ball_p_y - player_1_p) * ball_dp_x;
					if (ball_p_y < (player_1_p - player_half_size_y * .4f) && ball_dp_y > -10) player_1_ddp = (ball_p_y - player_1_p) * ball_dp_x;
				}
				else {
					if (ball_p_y > (player_1_p + player_half_size_y * .4f) || ball_p_y < (player_1_p - player_half_size_y * .5f)) 
						player_1_ddp = (ball_p_y - player_1_p) * ball_dp_x;
				}
			}

			if (player_1_ddp > 4600) player_1_ddp = 4600;
			if (player_1_ddp < -4600) player_1_ddp = -4600;
		}

		float player_2_ddp = 0.f; // acceleration
		if (is_down(BUTTON_W)) player_2_ddp += 4000;
		if (is_down(BUTTON_S)) player_2_ddp -= 4000;

		simulate_player(&player_1_p, &player_1_dp, player_1_ddp, dt);
		simulate_player(&player_2_p, &player_2_dp, player_2_ddp, dt);

		// Simulate ball (make into function if multiple balls required)
		{
			ball_p_x += ball_dp_x * dt;
			ball_p_y += ball_dp_y * dt;

			if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 800, player_1_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = 800 - player_half_size_x - ball_half_size;
				ball_dp_x *= -1.03f;
				ball_dp_y = (ball_p_y - player_1_p) * 2 + player_1_dp;
			}
			else if (aabb_vs_aabb(ball_p_x, ball_p_y, ball_half_size, ball_half_size, -800, player_2_p, player_half_size_x, player_half_size_y)) {
				ball_p_x = player_half_size_x + ball_half_size - 800;
				ball_dp_x *= -1.03f;
				ball_dp_y = (ball_p_y - player_2_p) * 2 + player_2_dp;
			}
			else if (ball_p_y + ball_half_size > arena_half_size_y) {
				ball_p_y = arena_half_size_y - ball_half_size;
				ball_dp_y *= -1;
			}
			else if (ball_p_y - ball_half_size < -arena_half_size_y) {
				ball_p_y = ball_half_size - arena_half_size_y;
				ball_dp_y *= -1;
			}
			else if (ball_p_x + ball_half_size > arena_half_size_x) {
				ball_p_x = 0;
				ball_p_y = 0;
				ball_dp_y = 0;
				ball_dp_x = 700;
				ball_dp_x *= -1;
				player_2_score += 1;
			}
			else if (ball_p_x - ball_half_size < -arena_half_size_x) {
				ball_p_x = 0;
				ball_p_y = 0;
				ball_dp_y = 0;
				ball_dp_x = 700;
				ball_dp_x *= -1;
				player_1_score += 1;
			}
		}



		// rendering

		// scores
		draw_number(player_1_score, 100, 40, 10.f, 0xcd3c29);
		draw_number(player_2_score, -100, 40, 10.f, 0xcd3c29);

		// ball
		draw_rect(ball_p_x, ball_p_y, ball_half_size, ball_half_size, 0xcd3c29);

		// players
		draw_rect(800, player_1_p, player_half_size_x, player_half_size_y, 0xb17a08);
		draw_rect(-800, player_2_p, player_half_size_x, player_half_size_y, 0xb17a08);

	}
	else {

		// Menu

		if (pressed(BUTTON_LEFT) || pressed(BUTTON_RIGHT)) {

			hot_button = !hot_button;
		
		}

		

		if (pressed(BUTTON_ENTER)) {
			current_gamemode = GM_GAMEPLAY;
			enemy_is_ai = hot_button ? 0 : 1;
		}

		if (hot_button == 0) {
			draw_rect(-400, 40, 20, 20, 0xffffff);
			draw_text("SINGLE PLAYER", -820, -10, 11, 0xcd3c29);
			draw_text("MULTIPLAYER", 100, -10, 9, 0xb17a08);
		}
		else {
			draw_rect(400, 40, 20, 20, 0xffffff);
			draw_text("MULTIPLAYER", 50, -10, 11, 0xcd3c29);
			draw_text("SINGLE PLAYER", -800, -10, 9, 0xb17a08);
		}


		// Menu text
		draw_text("PONG", -230, 380, 20, 0xffffff);
		draw_text("CODED BY KESSEM QUARKS", -300, 220, 5, 0xffffff);
		draw_text("BASED ON THE STEP BY STEP GUIDE", -300, 170, 3, 0xffffff);
		draw_text("BY DAN ZAIDAN ON YOUTUBE", -300, 140, 3, 0xffffff);


	}
	
}