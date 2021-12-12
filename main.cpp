/*
* Made in 48hrs by Isaiah Parton for Mini Jame Game of December 2021
* Code quality: absolutely scuffed
*/

#define GLSL_VERSION 330;
#define PLATFORM_DESKTOP;

#include <raylib.h>
#include <rlgl.h>
#include <raymath.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <cmath>
#include <array>
#include <map>
#include <chrono>

struct v2
{
public:
	v2(float _x = 0.0f, float _y = 0.0f)
	{
		x = _x;
		y = _y;
	}
	float x, y = 0.0f;
	float Dot(const v2& other)
	{
		return (x * other.x) + (y * other.y);
	}
	v2 Invert()
	{
		return v2(-x, -y);
	}
	v2 Round(const float factor)
	{
		return v2(round(x / factor) * factor, round(y / factor) * factor);
	}
	v2 Floor(const float factor)
	{
		return v2(floor(x / factor) * factor, floor(y / factor) * factor);
	}
	v2 Abs() const
	{
		return v2(abs(x), abs(y));
	}
	float Length() const
	{
		return sqrt((x * x) + (y * y));
	}
	v2 Multiply(const float value) const
	{
		return v2(x * value, y * value);
	}
	v2 operator +(const v2& other) const
	{
		return v2(x + other.x, y + other.y);
	}
	v2 operator +(const float& value) const
	{
		return v2(x + value, y + value);
	}
	v2 operator -(const v2& other) const
	{
		return v2(x - other.x, y - other.y);
	}
	v2 operator -(const float& value) const
	{
		return v2(x - value, y - value);
	}
	void operator +=(const v2& other)
	{
		x += other.x;
		y += other.y;
	}
	void operator -=(const v2& other)
	{
		x -= other.x;
		y -= other.y;
	}
	v2 operator *(const v2& other) const
	{
		return v2(x * other.x, y * other.y);
	}
	v2 operator *(const float& factor) const
	{
		return v2(x * factor, y * factor);
	}
	v2 operator /(const v2& other) const
	{
		return v2(x / other.x, y / other.y);
	}
	v2 operator /(const float& factor) const
	{
		return v2(x / factor, y / factor);
	}
	bool operator ==(const v2& other) const
	{
		return (x == other.x && y == other.y);
	}
	bool operator !=(const v2& other) const
	{
		return (x != other.x || y != other.y);
	}
};

typedef struct {
    Vector2 waveRange;
    Vector2 waveSpeed;
    Vector2 waveOffset;
} WaveTextConfig;

void DrawTextCodepoint3D(Font font, int codepoint, Vector3 position, float fontSize, bool backface, Color tint)
{
    // Character index position in sprite font
    // NOTE: In case a codepoint is not available in the font, index returned points to '?'
    int index = GetGlyphIndex(font, codepoint);
    float scale = fontSize/(float)font.baseSize;

    // Character destination rectangle on screen
    // NOTE: We consider charsPadding on drawing
    position.x += (float)(font.glyphs[index].offsetX - font.glyphPadding)/(float)font.baseSize*scale;
    position.z += (float)(font.glyphs[index].offsetY - font.glyphPadding)/(float)font.baseSize*scale;

    // Character source rectangle from font texture atlas
    // NOTE: We consider chars padding when drawing, it could be required for outline/glow shader effects
    Rectangle srcRec = { font.recs[index].x - (float)font.glyphPadding, font.recs[index].y - (float)font.glyphPadding,
                         font.recs[index].width + 2.0f*font.glyphPadding, font.recs[index].height + 2.0f*font.glyphPadding };

    float width = (float)(font.recs[index].width + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;
    float height = (float)(font.recs[index].height + 2.0f*font.glyphPadding)/(float)font.baseSize*scale;

    if (font.texture.id > 0)
    {
        const float x = 0.0f;
        const float y = 0.0f;
        const float z = 0.0f;

        // normalized texture coordinates of the glyph inside the font texture (0.0f -> 1.0f)
        const float tx = srcRec.x/font.texture.width;
        const float ty = srcRec.y/font.texture.height;
        const float tw = (srcRec.x+srcRec.width)/font.texture.width;
        const float th = (srcRec.y+srcRec.height)/font.texture.height;

        rlCheckRenderBatchLimit(4 + 4*backface);
        rlSetTexture(font.texture.id);

        rlPushMatrix();
            rlTranslatef(position.x, position.y, position.z);

            rlBegin(RL_QUADS);
                rlColor4ub(tint.r, tint.g, tint.b, tint.a);

                // Front Face
                rlNormal3f(0.0f, 1.0f, 0.0f);                                   // Normal Pointing Up
                rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);              // Top Left Of The Texture and Quad
                rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height);     // Bottom Left Of The Texture and Quad
                rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height);     // Bottom Right Of The Texture and Quad
                rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);              // Top Right Of The Texture and Quad

                if (backface)
                {
                    // Back Face
                    rlNormal3f(0.0f, -1.0f, 0.0f);                              // Normal Pointing Down
                    rlTexCoord2f(tx, ty); rlVertex3f(x,         y, z);          // Top Right Of The Texture and Quad
                    rlTexCoord2f(tw, ty); rlVertex3f(x + width, y, z);          // Top Left Of The Texture and Quad
                    rlTexCoord2f(tw, th); rlVertex3f(x + width, y, z + height); // Bottom Left Of The Texture and Quad
                    rlTexCoord2f(tx, th); rlVertex3f(x,         y, z + height); // Bottom Right Of The Texture and Quad
                }
            rlEnd();
        rlPopMatrix();

        rlSetTexture(0);
    }
}void DrawText3D(Font font, const char *text, Vector3 position, float fontSize, float fontSpacing, float lineSpacing, bool backface, Color tint)
{
    int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;               // Offset X to next character to draw

    float scale = fontSize/(float)font.baseSize;

    for (int i = 0; i < length;)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;

        if (codepoint == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += scale + lineSpacing/(float)font.baseSize*scale;
            textOffsetX = 0.0f;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                DrawTextCodepoint3D(font, codepoint, (Vector3){ position.x + textOffsetX, position.y, position.z + textOffsetY }, fontSize, backface, tint);
            }

            if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing)/(float)font.baseSize*scale;
            else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing)/(float)font.baseSize*scale;
        }

        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }
}
Vector3 MeasureText3D(Font font, const char* text, float fontSize, float fontSpacing, float lineSpacing)
{
    int len = TextLength(text);
    int tempLen = 0;                // Used to count longer text line num chars
    int lenCounter = 0;

    float tempTextWidth = 0.0f;     // Used to count longer text line width

    float scale = fontSize/(float)font.baseSize;
    float textHeight = scale;
    float textWidth = 0.0f;

    int letter = 0;                 // Current character
    int index = 0;                  // Index position in sprite font

    for (int i = 0; i < len; i++)
    {
        lenCounter++;

        int next = 0;
        letter = GetCodepoint(&text[i], &next);
        index = GetGlyphIndex(font, letter);

        // NOTE: normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol so to not skip any we set next = 1
        if (letter == 0x3f) next = 1;
        i += next - 1;

        if (letter != '\n')
        {
            if (font.glyphs[index].advanceX != 0) textWidth += (font.glyphs[index].advanceX+fontSpacing)/(float)font.baseSize*scale;
            else textWidth += (font.recs[index].width + font.glyphs[index].offsetX)/(float)font.baseSize*scale;
        }
        else
        {
            if (tempTextWidth < textWidth) tempTextWidth = textWidth;
            lenCounter = 0;
            textWidth = 0.0f;
            textHeight += scale + lineSpacing/(float)font.baseSize*scale;
        }

        if (tempLen < lenCounter) tempLen = lenCounter;
    }

    if (tempTextWidth < textWidth) tempTextWidth = textWidth;

    Vector3 vec = { 0 };
    vec.x = tempTextWidth + (float)((tempLen - 1)*fontSpacing/(float)font.baseSize*scale); // Adds chars spacing to measure
    vec.y = 0.25f;
    vec.z = textHeight;

    return vec;
}
void DrawTextWave(const char *text, int xPos, int yPos, int fontSize, float fontSpacing, float lineSpacing, WaveTextConfig *config, float time, Color tint)
{
	Font font = GetFontDefault();
	int length = TextLength(text);          // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0.0f;               // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;               // Offset X to next character to draw

    float scale = fontSize/(float)font.baseSize;

    bool wave = true;

    for (int i = 0, k = 0; i < length; ++k)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f) codepointByteCount = 1;

        if (codepoint == '\n')
        {
            // NOTE: Fixed line spacing of 1.5 line-height
            // TODO: Support custom line spacing defined by user
            textOffsetY += scale + lineSpacing/(float)font.baseSize*scale;
            textOffsetX = 0.0f;
            k = 0;
        }
        else
        {
            if ((codepoint != ' ') && (codepoint != '\t'))
            {
                Vector2 pos = {(float)xPos, (float)yPos};
                if (wave) // Apply the wave effect
                {
                    pos.x += sin(time*config->waveSpeed.x-k*config->waveOffset.x)*config->waveRange.x;
                    pos.y += sin(time*config->waveSpeed.y-k*config->waveOffset.y)*config->waveRange.y;
                }

                DrawTextCodepoint(font, codepoint, (Vector2){pos.x + (float)textOffsetX, pos.y + (float)textOffsetY}, fontSize, tint);
            }

            if (font.glyphs[index].advanceX == 0) textOffsetX += (float)(font.recs[index].width + fontSpacing)/(float)font.baseSize*scale;
            else textOffsetX += (float)(font.glyphs[index].advanceX + fontSpacing)/(float)font.baseSize*scale;
        }

        i += codepointByteCount;   // Move text bytes counter to next codepoint
    }
}

v2 CELL_SIZE = {40.0f, 40.0f};
const int LEVEL_WIDTH = 20;
const int LEVEL_HEIGHT = 20;
const int INFO_HEIGHT = 50;
const int SCREEN_WIDTH = LEVEL_WIDTH * CELL_SIZE.x;
const int SCREEN_HEIGHT = LEVEL_HEIGHT * CELL_SIZE.y + INFO_HEIGHT;
const int TARGET_FPS = 60;
const float MOVEMENT_SPEED = 30;
const char* GAME_TITLE = "QUADRATUS";
const bool ENABLE_SOUND = true;
bool showHelp = true;
bool inGame = false;
bool isPaused = false;
int gameState = 2;
float deltaTime = 0.0f;
float now = 0.0f;
v2 camera = {0.0f, 0.0f};
float cameraShake_factor = 0.0f;
float cameraShake_time = 0.0f;
void ShakeCamera(float factor, float time)
{
	cameraShake_factor = factor;
	cameraShake_time = time;
}
std::map<std::string, std::vector<Sound>> sounds;
std::vector<std::pair<std::string, std::vector<const char*>>> soundsToLoad{
	{"key_yoink", {"assets/sounds/key_yoink.wav"}},
	{"death", {"assets/sounds/death.wav"}},
	{"alert", {"assets/sounds/alert.wav"}},
	{"win", {"assets/sounds/win.wav"}},
	{"unlocked", {"assets/sounds/unlocked.wav"}},
	{"intro", {"assets/sounds/intro.wav"}},
	{"title_screen", {"assets/sounds/title_screen.wav"}},
	{"game_start", {"assets/sounds/game_start.wav"}},
	{"game_over", {"assets/sounds/game_over.wav"}},
	{"select", {"assets/sounds/select.wav"}},
	{"move", {"assets/sounds/move-1.wav", "assets/sounds/move-2.wav"}},
	{"push", {"assets/sounds/push.wav"}},
	{"impact", {"assets/sounds/impact.wav"}},
	{"melody", {"assets/sounds/melody-1.wav", "assets/sounds/melody-2.wav", "assets/sounds/melody-2.wav"}},
	{"melody_chaos", {"assets/sounds/melody_chaos-1.wav", "assets/sounds/melody_chaos-2.wav", "assets/sounds/melody_chaos-3.wav"}}
};
void PlayGameSound(std::string name, float volume = 1.0f, float pitch = 1.0f)
{
	if(!ENABLE_SOUND)
		return;
	Sound snd = sounds[name][GetRandomValue(0, sounds[name].size() - 1)];
	SetSoundVolume(snd, volume);
	SetSoundPitch(snd, pitch);
	PlaySoundMulti(snd);
}
const int MAX_WAVES = 256;
const float WAVE_SPEED = 0.5f;
bool  wave_exists    [MAX_WAVES] = {false};
v2    wave_position  [MAX_WAVES] = {0.0f, 0.0f};
int   wave_direction [MAX_WAVES] = {0};
float wave_life      [MAX_WAVES] = {0.0f};
Color Rainbow()
{
	return ColorFromHSV((float)(sin(now / 4) * 360), 0.8f, 0.9f);
}
void AddWave(v2 position, float direction)
{
	for(int i = 0; i < MAX_WAVES; i++)
	{
		if(!wave_exists[i])
		{
			wave_exists[i] = true;
			wave_position[i] = position;
			wave_direction[i] = direction;
			wave_life[i] = 0.0f;
			return;
		}
	}
}
void DrawCenteredText(const char* text, int posX, int posY, int size, Color color)
{
	int offset = MeasureText(text, size);
	DrawText(text, posX - offset / 2, posY, size, color);
}

struct Cell
{
public:
	Cell(int _type, v2 _position)
	{
		type = _type;
		trail = _position;
		position = _position;
		if(type == 1)
		{
			color = BLACK;
		}
		else if(type == 2)
		{
			helpText = "This is you";
			color = WHITE;
			isStatic = false;
		}
		else if(type == 3)
		{
			helpText = "Boi";
			color = BLUE;
			isStatic = false;
		}
		else if(type == 4)
		{
			helpText = "This stops you";
			doesCollide = false;
		}
		else if(type == 6)
		{
			helpText = "This is the exit";
			doesCollide = true;
			color = GOLD;
		}
		else if(type == 7)
		{
			helpText = "This bounces things";
			doesCollide = true;
		}
		else if(type == 8)
		{
			helpText = "Only bois can break this";
			doesCollide = true;
			color = DARKGRAY;
		}
	};
	int type;
	v2 screenPos;
	v2 position;
	v2 velocity;
	v2 gridPosition;
	v2 movementNormal;
	bool isStatic = true;
	bool isMoving = false;
	bool doesCollide = true;
	bool booped = false;
	bool locked = true;
	const char* helpText = "";
	int direction = 0;
	float moveCooldown = 0;
	bool mustDestroy = false;
	Color color;
	Cell* held;
	v2 trail;
	void Draw(v2 origin)
	{
		v2 size = {CELL_SIZE.x, CELL_SIZE.y};
		v2 center = origin + size / 2;
		v2 trailDelta = trail.Abs() * movementNormal.Abs();
		size += trailDelta;
		origin -= trailDelta;
		if(type == 3)
		{
			DrawRectangle(origin.x, origin.y, size.x, size.y, Fade(Rainbow(), 0.5f));
		}
		else if(type == 7)
		{
			DrawRectangle(origin.x, origin.y, size.x, size.y, MAROON);
			DrawRectangleLines(origin.x, origin.y, size.x, size.y, WHITE);
			float rs = abs(sin(now * 2)) * 40;
			DrawRectangleLines(center.x + 1 - rs / 2, center.y + 1 - rs / 2, rs, rs, WHITE);
			float rs2 = 40 - abs(sin(now * 2)) * 40;
			DrawRectangleLines(center.x + 1 - rs2 / 2, center.y + 1 - rs2 / 2, rs2, rs2, WHITE);
		}
		else if(type != 4)
		{
			DrawRectangle(origin.x, origin.y, size.x, size.y, color);
		}
		if(type == 2)
		{
			DrawRectangleLines(origin.x, origin.y, size.x, size.y, BLACK);
		}
		if(type == 3)
		{
			DrawRectangleLines(origin.x, origin.y, size.x, size.y, WHITE);
			if(isMoving)
			{
				DrawRectangle(origin.x + size.x * 0.4, origin.y + size.y * 0.55, size.x * 0.2, size.y * 0.3, WHITE);
			}
			else
			{
				DrawRectangle(origin.x + size.x * 0.2, origin.y + size.y * 0.6, size.x * 0.6, size.y * 0.05, WHITE);
			}
			DrawRectangle(origin.x + size.x * 0.2, origin.y + size.y * 0.4, size.x * 0.1, size.y * 0.1, WHITE);
			DrawRectangle(origin.x + size.x - size.x * 0.2 - size.x * 0.1, origin.y + size.y * 0.4, size.x * 0.1, size.y * 0.1, WHITE);
		}
		if(type == 4)
		{
			Color clr = Fade(Rainbow(), 0.3f);
			DrawRectangle(origin.x, origin.y, size.x, size.y, clr);
			DrawRectangleLines(origin.x, origin.y, size.x, size.y, WHITE);
		}
		if(type == 6)
		{
			if(doesCollide)
			{
				DrawRectangle(origin.x, origin.y, CELL_SIZE.x, CELL_SIZE.y, BLACK);
				DrawRectangle(center.x - 6, center.y - 10, 12, 12, GOLD);
				DrawRectangle(center.x - 3, center.y, 6, 12, GOLD);
			}
		}
	}
	void Snap()
	{
		position = position.Round(1);
	}
	void Stop()
	{
		velocity = {0.0f, 0.0f};
		movementNormal = {0.0f, 0.0f};
		trail = {0.0f, 0.0f};
		isMoving = false;
		PlayGameSound("impact", 0.3f);
	}
	void Update()
	{
		if(moveCooldown > 0)
			moveCooldown -= deltaTime;
		if(!isStatic)
		{
			position += velocity * deltaTime;
		}
		gridPosition = position.Round(1);
		screenPos = position * CELL_SIZE;
		trail += (velocity - trail) * 10 * deltaTime;
	}
	void Destroy()
	{
		ShakeCamera(15.0f, 0.5f);
		AddWave({screenPos.x + 20, screenPos.y}, 0);
		AddWave({screenPos.x + 20, screenPos.y + 40}, 1);
		AddWave({screenPos.x, screenPos.y + 20}, 2);
		AddWave({screenPos.x + 40, screenPos.y + 20}, 3);
		mustDestroy = true;
	}
};

v2 lock_position = {0.0f, 0.0f};
v2 key_position = {0.0f, 0.0f};
v2 key_velocity = {0.0f, 0.0f};
float key_rotation = 0.0f;
float key_scale = 0.4f;
bool key_yoinked = false;
bool key_enabled = true;
float key_timer = 0.0f;
float game_timer = 0.0f;
float screenFade;
float deathTimer = 0.0f;
bool isDead = false;
std::vector<Cell*> cells;
Cell* player_cell;

float ambientTimer = 5.0f;
float logoTimer = 1.8f;
float logoAlpha = 1.0f;
float alertTimer = 0.0f;
float restartTimer = 0.0f;
bool mustRestart = false;
int level = 0;
static const std::vector<std::vector<std::vector<int>>> levels = {
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,1},
		{1,0,1,1,1,1,1,0,0,1,0,0,1,1,1,1,1,1,0,1},
		{1,0,1,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,0,1,1,6,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1},
		{1,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,5,0,0,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,3,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,3,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	},
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,7,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,2,1,1,1,1,1,1,1,1,1,6,1,1,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
		{1,1,1,1,4,0,0,0,0,0,0,0,0,0,0,0,7,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,3,1,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,5,1,1,1,1},
		{1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	},
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,0,0,1,1,1,1,0,0,0,4,0,0,0,2,1,1,1,1,1},
		{1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,6,1,1,1,1},
		{1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1},
		{1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1},
		{1,0,0,0,0,0,8,0,0,0,3,0,0,0,4,1,1,1,1,1},
		{1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1},
		{1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1},
		{1,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,3,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
		{1,1,1,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,1,1},
		{1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1,7,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	},
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,7,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,4,0,0,0,0,1,1,1,1,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1},
		{1,3,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,2,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,3,1,1,1,0,1,1,1,1},
		{1,8,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
		{1,0,0,0,0,0,0,4,0,0,8,0,0,0,0,4,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,8,1,1,1,0,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
		{1,0,1,1,1,1,1,0,0,5,0,0,1,1,1,0,1,1,1,1},
		{1,0,1,1,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1},
		{1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{1,0,6,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	},
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,7,1,1,1,1,1,1,1,1,1,1,1,1,7,1,1,1,1},
		{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1},
		{1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,3,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,4,0,1,1},
		{1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,0,1,1,1,1,0,0,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,0,0,0,3,0,8,0,4,0,1,1},
		{1,1,0,1,1,1,1,1,1,1,1,1,1,1,6,1,0,0,1,1},
		{1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1},
		{1,1,0,0,0,5,1,1,1,1,1,1,1,1,1,1,0,0,1,1},
		{1,1,3,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1},
		{1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1},
		{1,1,2,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	},
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,1,5,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,1,1,1,8,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1},
		{1,7,0,0,0,0,4,0,3,0,4,0,3,0,4,1,1,1,1,1},
		{1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1},
		{1,1,0,1,1,1,3,1,1,1,0,1,1,1,0,1,1,1,1,1},
		{1,1,0,1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1,1},
		{1,1,0,0,0,0,4,0,0,0,4,0,0,0,2,1,1,1,1,1},
		{1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1},
		{1,1,1,1,1,1,0,1,1,1,1,1,1,1,0,1,1,1,1,1},
		{1,1,1,1,1,1,0,6,1,1,1,1,1,1,0,1,1,1,1,1},
		{1,1,1,1,1,7,0,0,0,0,0,0,0,0,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	},
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,5,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,7,1},
		{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,1},
		{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,1},
		{1,1,0,1,1,1,1,3,1,1,1,1,8,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,0,0,0,0,0,4,0,0,0,0,4,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,2,0,0,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	},
	{
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,0,0,0,5,0,0,0,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,7,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1,1},
		{1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1},
		{1,1,0,0,0,0,4,0,3,0,4,0,0,0,0,0,0,0,2,1},
		{1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
		{1,1,0,1,1,1,3,1,1,1,0,0,3,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,0,1,1,1,0,0,0,1,1,1,1,1,1,1},
		{1,1,0,1,1,1,0,1,1,1,1,0,1,1,1,1,1,1,1,1},
		{1,1,0,0,0,0,0,1,1,1,1,5,1,1,1,1,1,1,1,1},
		{1,1,0,0,6,0,0,1,1,1,1,7,1,1,1,1,1,1,1,1},
		{1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
	}
};
void LoadLevel(int index)
{
	if(index >= levels.size())
		return;
	cells.clear();
	for(int y = 0; y < LEVEL_HEIGHT; y++)
	{
		for(int x = 0; x < LEVEL_WIDTH; x++)
		{
			int cell = levels[index][y][x];
			v2 pos = {(float)x, (float)y};
			if(cell == 2)
			{
				player_cell = new Cell(2, pos);
				cells.push_back(player_cell);
			}
			else if(cell == 5)
			{
				key_position = (pos + v2(0.5f, 0.5f)) * CELL_SIZE;
				key_velocity = {0.0f, 0.0f};
				key_yoinked = false;
				key_scale = 0.4f;
				key_enabled = true;
				key_velocity = {0.0f, 0.0f};
			}
			else if(cell > 0)
			{
				cells.push_back(new Cell(cell, pos));
				if(cell == 6)
				{
					lock_position = (pos + v2(0.5f, 0.5f)) * CELL_SIZE;
				}
			}
		}
	}
}
Cell* GetCellAt(v2 gridPosition)
{
	for(int i = 0; i < cells.size(); i++)
	{
		if(cells[i]->gridPosition == gridPosition)
		{
			return cells[i];
		}
	}
	return nullptr;
}
void Push(Cell* self, int _direction, float _speed)
{
	if(self->moveCooldown > 0)
		return;
	v2 norm = {0.0f, 0.0f};
	if(_direction == 0)
		norm.y = -1;
	else if(_direction == 1)
		norm.y = 1;
	else if(_direction == 2)
		norm.x = -1;
	else if(_direction == 3)
		norm.x = 1;
	Cell* neighboor = GetCellAt(self->gridPosition + norm);
	if(self->gridPosition + norm != self->gridPosition && neighboor != nullptr && neighboor->doesCollide && neighboor->type != 3)
	{
		return;
	}
	if(self->type == 2)
	{
		//PlayGameSound("move");
	}
	else
	{
		PlayGameSound("push");
	}
	self->isMoving = true;
	self->direction = _direction;
	self->movementNormal = norm;
	self->velocity = norm * _speed;
};

void FinishLevel()
{
	level++;
	if(level >= levels.size())
	{
		inGame = false;
		gameState = 4;
		PlayGameSound("win");
		level--;
	}
	else
	{
		game_timer += 15;
		LoadLevel(level);
	}
}
void ScreenFadeTo(float value)
{
	screenFade += (value - screenFade) * 10 * deltaTime;
}
void DrawKey(int x, int y, float scale, float rotation)
{
	Color clr = YELLOW;
	rlPushMatrix();
	rlTranslatef(x, y, 0.0f);
	rlScalef(scale, scale, 0.0f);
	rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
	DrawRectangle(15, -20, 40, 10, clr);
	DrawRectangle(15, -20, 10, 40, clr);
	DrawRectangle(15, 10, 40, 10, clr);
	DrawRectangle(45, -20, 10, 40, clr);
	DrawRectangle(-45, -5, 60, 10, clr);
	DrawRectangle(-45, 0, 20, 16, clr);
	rlPopMatrix();
}
void ResetGame()
{
	level = 0;
	game_timer = 0.0f;
}
void StartGame()
{
	game_timer = 30.0f;
}

int main()
{
	InitAudioDevice();
	
	for(int i = 0; i < soundsToLoad.size(); i++)
	{
		std::vector<Sound> pool = {};
		for(int j = 0; j < soundsToLoad[i].second.size(); j++)
		{
			pool.push_back(LoadSound(soundsToLoad[i].second[j]));
		}
		sounds[soundsToLoad[i].first] = pool;
	}
	Camera3D otherCamera{0};
	otherCamera.position = (Vector3){0.0f, 0.0f, 10.0f};
	otherCamera.target = (Vector3){ 0.0f, 0.0f, 0.0f };          
	otherCamera.up = (Vector3){ 0.0f, 1.0f, 0.0f };              
	otherCamera.fovy = 45.0f;                                    
	otherCamera.projection = CAMERA_PERSPECTIVE;
	WaveTextConfig wcfg{0};
	wcfg.waveSpeed.x = wcfg.waveSpeed.y = 4.0f;
	wcfg.waveOffset.x = -100.5f; wcfg.waveOffset.y = 1.0f;
	wcfg.waveRange.x = 200.0f; wcfg.waveRange.y = 12.0f;
	float titleOffset = -(float)SCREEN_WIDTH;
	WaveTextConfig wcfg2{0};
	wcfg2.waveSpeed.x = wcfg2.waveSpeed.y = 4.7f;
	wcfg2.waveOffset.x = 0.5f; wcfg2.waveOffset.y = 0.4f;
	wcfg2.waveRange.x = 0.0f; wcfg2.waveRange.y = 4.0f;
	
	SetConfigFlags(FLAG_MSAA_4X_HINT); 
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, GAME_TITLE);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	SetWindowIcon(LoadImage("assets/textures/icon.png"));

	PlayGameSound("intro");
	
	while(!WindowShouldClose())
	{
		if(!IsWindowFocused() || IsWindowResized() || !IsWindowReady())
			isPaused = true;
		deltaTime = GetFrameTime();
		if(deltaTime > 1.0f)
		{
			deltaTime = 1.0f;
		}
		now += deltaTime;

		ClearBackground(BLACK);
		if(gameState == 0)
		{
			BeginDrawing();
			int ts = MeasureText(GAME_TITLE, 80);
			wcfg.waveRange.x += (5.0f - wcfg.waveRange.x) * 0.5 * deltaTime;
			titleOffset -= titleOffset * 5 * deltaTime;
			DrawTextWave(GAME_TITLE, SCREEN_WIDTH / 2 - ts / 2 + titleOffset - 5, SCREEN_HEIGHT / 2 - 195, 80, 75, 1, &wcfg, now, GRAY);
			DrawTextWave(GAME_TITLE, SCREEN_WIDTH / 2 - ts / 2 + titleOffset, SCREEN_HEIGHT / 2 - 200, 80, 75, 1, &wcfg, now, WHITE);
			DrawCenteredText(" Start Game\n  [ ENTER ]", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, WHITE);
			DrawCenteredText("[ H ] How to play", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 30 - abs(cos(now) * 50), 20, WHITE);
			DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, screenFade));
			EndDrawing();
			screenFade -= screenFade * 10 * deltaTime;
			if(IsKeyPressed(KEY_ENTER))
			{
				inGame = true;
				isPaused = false;
				gameState = 1;
				PlayGameSound("select");
				StartGame();
				LoadLevel(0);
			}
			if(IsKeyPressed(KEY_H))
			{
				gameState = 3;
				PlayGameSound("select");
			}
		}
		else if(gameState == 1)
		{
			if(IsKeyPressed(KEY_SPACE))
			{
				PlayGameSound("select");
				isPaused = !isPaused;
			}
			if(IsKeyPressed(KEY_R))
			{
				PlayGameSound("select");
				mustRestart = true;
				restartTimer = 1.0f;
			}
			if(restartTimer > 0)
				restartTimer -= deltaTime;
			if(restartTimer < 0.5f)
			{
				if(mustRestart)
				{
					LoadLevel(level);
					mustRestart = false;
				}
				ScreenFadeTo(0.0f);
			}
			else
			{
				ScreenFadeTo(255.0f);
			}
			if(!player_cell->isMoving && !isPaused && !showHelp)
			{
				if(IsKeyPressed(KEY_UP))
				{
					Push(player_cell, 0, MOVEMENT_SPEED);
				}
				else if(IsKeyPressed(KEY_DOWN))
				{
					Push(player_cell, 1, MOVEMENT_SPEED);
				}
				if(IsKeyPressed(KEY_LEFT))
				{
					Push(player_cell, 2, MOVEMENT_SPEED);
				}
				else if(IsKeyPressed(KEY_RIGHT))
				{
					Push(player_cell, 3, MOVEMENT_SPEED);
				}
			}
			if(cameraShake_time > 0)
			{
				cameraShake_time -= deltaTime;
			}
			else
			{
				cameraShake_factor = 0;
			}

			// key business
			if(key_enabled)
			{
				float distanceToKey = v2(player_cell->screenPos.x + CELL_SIZE.x / 2 - key_position.x, player_cell->screenPos.y + CELL_SIZE.y / 2 - key_position.y).Length();
				if(distanceToKey < 20 && !key_yoinked)
				{
					PlayGameSound("key_yoink");
					key_yoinked = true;
				}
				if(key_yoinked)
				{
					float distanceToLock = v2(lock_position.x - key_position.x, lock_position.y - key_position.y).Length();
					if(distanceToLock > 10)
					{
						key_timer = 0.0f;
						key_velocity += ((lock_position - key_position) * 3 - key_velocity) * 10 * deltaTime;
						key_scale += (0.6f - key_scale) * 5 * deltaTime;
					}
					else
					{
						key_scale -= key_scale * 3 * deltaTime;
						key_velocity -= key_velocity * 10 * deltaTime;
						key_rotation += sin(now / 2) * 5;
						key_timer += deltaTime;
						if(key_timer > 0.8f)
						{
							for(int i = 0; i < cells.size(); i++)
							{
								if(cells[i]->type == 6)
								{
									AddWave({cells[i]->screenPos.x + 20, cells[i]->screenPos.y}, 0);
									AddWave({cells[i]->screenPos.x + 20, cells[i]->screenPos.y + 40}, 1);
									AddWave({cells[i]->screenPos.x, cells[i]->screenPos.y + 20}, 2);
									AddWave({cells[i]->screenPos.x + 40, cells[i]->screenPos.y + 20}, 3);
									ShakeCamera(20.0f, 0.3f);
									PlayGameSound("unlocked");
									key_enabled = false;
									cells[i]->doesCollide = false;
									i = cells.size();
								}
							}
						}
					}
				}
				else
				{
					key_rotation += abs(sin(now * 2)) * 2;
				}
			}
			key_position += key_velocity * deltaTime;
			float shakeFactor = cameraShake_factor * cameraShake_time;

			// cell business
			ClearBackground({25, 25, 25});
			BeginDrawing();
				rlPushMatrix();
				rlTranslatef(camera.x + GetRandomValue(-shakeFactor, shakeFactor), camera.y + GetRandomValue(-shakeFactor, shakeFactor), 0.0f);
					player_cell->Draw(player_cell->position * CELL_SIZE);
					for(int i = 0; i < cells.size(); i++)
					{
						if(cells[i]->mustDestroy)
						{
							cells.erase(cells.begin() + i);
							continue;
						}
						Cell* A = cells[i];
						if(!isPaused)
							A->Update();
						
						// fizix
						if(A->doesCollide){
						for(int j = 0; j < cells.size(); j++)
						{
							Cell* B = cells[j];
							bool impact = false;
							bool push = false;
							v2 newA = A->position + A->velocity * deltaTime;
							if(!B->doesCollide)
							{
								if(B->type == 4)
								{
									if(B->booped)
									{
										if(B->held->gridPosition != B->gridPosition)
											B->booped = false;
									}
									else if(A->isMoving && A->gridPosition == B->gridPosition)
									{
										AddWave({B->screenPos.x + 20, B->screenPos.y}, 0);
										AddWave({B->screenPos.x + 20, B->screenPos.y + 40}, 1);
										AddWave({B->screenPos.x, B->screenPos.y + 20}, 2);
										AddWave({B->screenPos.x + 40, B->screenPos.y + 20}, 3);
										A->Stop();
										A->Snap();
										B->booped = true;
										B->held = A;
									}
								}
								else if(B->type == 6)
								{
									if(A->isMoving && player_cell->gridPosition == B->gridPosition)
									{
										A->Stop();
										A->Snap();
										FinishLevel();
									}
								}
							}
							else if(A != B && B->doesCollide && A->isMoving)
							{
								if(A->gridPosition.x == B->gridPosition.x && newA.y > B->position.y - 1 && newA.y < B->position.y + 1)
								{
									if(A->direction == 0 || A->direction == 1)
									{
										if(B->type == 8 && A->type != 2)
										{
											impact = true;
											B->Destroy();
										}
										else
										{
											if(!B->isStatic)
											{
												push = true;
												Push(B, A->direction, abs(A->velocity.y) / 2);
											}
											impact = true;
											float reboundSpeed = abs(A->velocity.y);
											A->Stop();
											A->Snap();
											if(B->type == 7)
											{
												Push(A, A->direction == 0 ? 1:0, reboundSpeed);
											}
										}
									}
								}
								if(A->gridPosition.y == B->gridPosition.y && newA.x > B->position.x - 1 && newA.x < B->position.x + 1)
								{
									if(A->direction == 2 || A->direction == 3)
									{
										if(B->type == 8 && A->type != 2)
										{
											impact = true;
											B->Destroy();
										}
										else
										{
											if(!B->isStatic)
											{
												push = true;
												Push(B, A->direction, abs(A->velocity.x) / 2);
											}
											impact = true;
											float reboundSpeed = abs(A->velocity.x);
											A->Stop();
											A->Snap();
											if(B->type == 7)
											{
												Push(A, A->direction == 2 ? 3:2, reboundSpeed);
											}
										}
									}
								}
							}
							if(impact)
							{
								ShakeCamera(10.0f, 0.3f);
								if(push)
								{
									AddWave({A->screenPos.x + 20, A->screenPos.y}, 0);
									AddWave({A->screenPos.x + 20, A->screenPos.y + 40}, 1);
									AddWave({A->screenPos.x, A->screenPos.y + 20}, 2);
									AddWave({A->screenPos.x + 40, A->screenPos.y + 20}, 3);
									ShakeCamera(20.0f, 0.3f);
								}
							}
						}
						}

						v2 drawPos = A->position * CELL_SIZE;
						if(A != player_cell && A->type != 1)
							A->Draw(drawPos);
					}
					for(int i = 0; i < cells.size(); i++)
					{
						if(cells[i]->type == 1)
							cells[i]->Draw(cells[i]->position * CELL_SIZE);
					}
					for(int i = 0; i < MAX_WAVES; i++)
					{
						if(wave_exists[i])
						{
							int size = 15 * (1 - wave_life[i]);
							if(wave_direction[i] == 0 || wave_direction[i] == 1)
							{
								wave_position[i].y += (wave_direction[i] == 0 ? -1:1) * (1.0f - wave_life[i]) * WAVE_SPEED;
								DrawLine(wave_position[i].x - size, wave_position[i].y, wave_position[i].x + size, wave_position[i].y, WHITE);
							}
							if(wave_direction[i] == 2 || wave_direction[i] == 3)
							{
								wave_position[i].x += (wave_direction[i] == 2 ? -1:1) * (1.0f - wave_life[i]) * WAVE_SPEED;
								DrawLine(wave_position[i].x, wave_position[i].y - size, wave_position[i].x, wave_position[i].y + size, WHITE);
							}
							wave_life[i] += 4 * deltaTime;
							if(wave_life[i] > 1.0f)
							{
								wave_exists[i] = false;
							}
						}
					}
					
				rlPopMatrix();
				if(showHelp)
				{
					DrawCenteredText("Press [H] to toggle help", SCREEN_WIDTH / 2, 10 + sin(now * 3), 30, WHITE);
					for(int i = 0; i < cells.size(); i++)
					{
						if(cells[i]->type > 1){
							v2 p = cells[i]->screenPos + CELL_SIZE / 2 + v2{0.0f, sin((now + i) * 3) * 3};
							DrawLine(p.x, p.y - 35, p.x, p.y - 25, WHITE);
							DrawCenteredText(cells[i]->helpText, p.x, p.y - 55, 20, WHITE);
						}
					}
					if(key_enabled)
						DrawCenteredText("Get the key!", key_position.x, key_position.y + 25, 20, Rainbow());
				}
				if(IsKeyPressed(KEY_H))
				{
					PlayGameSound("select");
					showHelp = !showHelp;
				}
				DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, screenFade));
				DrawRectangle(0, SCREEN_HEIGHT - INFO_HEIGHT, SCREEN_WIDTH, INFO_HEIGHT, WHITE);
				DrawText(TextFormat("Level %i/%i", level + 1, levels.size()), 12, SCREEN_HEIGHT - 38, 30, BLACK);
				const char* timeText = TextFormat("Time: %is", (int)floor(game_timer));
				int ts = MeasureText(timeText, 30);
				DrawText(timeText, SCREEN_WIDTH - 12 - ts, SCREEN_HEIGHT - 38, 30, BLACK);
				if(key_enabled)
					DrawKey(key_position.x, key_position.y, key_scale, key_rotation);
				if(isPaused)
				{
					ScreenFadeTo(1.0f);
					int ts1 = MeasureText("Paused", 40);
					int ts2 = MeasureText("[SPACE]", 30);
					DrawText("Paused", SCREEN_WIDTH / 2 - ts1 / 2, SCREEN_HEIGHT / 2 - 50, 40, WHITE);
					DrawText("[SPACE]", SCREEN_WIDTH / 2 - ts2 / 2, SCREEN_HEIGHT / 2, 30, WHITE);
				}
				else
				{
					if(!showHelp)
						game_timer -= deltaTime;
					if(game_timer <= 0)
					{
						gameState = 5;
						PlayGameSound("game_over");
					}
					else if(game_timer <= 5.0f)
					{
						if(alertTimer <= 0)
						{
							alertTimer = game_timer * 0.1;
							ShakeCamera(30.0f, 0.2f);
							PlayGameSound("alert", 0.5f, 2.0f + (5.0f - game_timer));
						}
						else
						{
							alertTimer -= deltaTime;
						}
					}
					if(ambientTimer < 0)
					{
						ambientTimer = GetRandomValue(10, 15);
						if(rand() % 100 > 50)
						{
							PlayGameSound("melody_chaos", 0.1f);
						}
						else
						{
							PlayGameSound("melody", 0.1f);
						}
					}
					else
					{
						ambientTimer -= deltaTime;
					}
					ScreenFadeTo(0.0f);
				}
			EndDrawing();
		}
		else if(gameState == 2)
		{
			logoTimer -= deltaTime;
			if(logoTimer <= 0)
			{
				gameState = 0;
				PlayGameSound("title_screen");
			}
			if(logoTimer < 0.3)
			{
				screenFade += (1.0f - screenFade) * 10 * deltaTime;
			}
			else
			{
				logoAlpha -= logoAlpha * 10 * deltaTime;
			}
			BeginDrawing();
				ClearBackground(RAYWHITE);
				DrawRectangle(SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT/2 - 128, 256, 256, BLACK);
				DrawRectangle(SCREEN_WIDTH/2 - 112, SCREEN_HEIGHT/2 - 112, 224, 224, RAYWHITE);
				DrawText("raylib", SCREEN_WIDTH/2 - 44, SCREEN_HEIGHT/2 + 48, 50, BLACK);
				DrawText("Made with", SCREEN_WIDTH/2 - 128, SCREEN_HEIGHT/2 - 150, 20, GRAY);
				DrawCenteredText("Sounds made with CHIPTONE", SCREEN_WIDTH/2, SCREEN_HEIGHT - 100, 30, GRAY);
				DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(RAYWHITE, logoAlpha));
				DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, screenFade));
			EndDrawing();
		}
		else if(gameState == 3)
		{
			BeginDrawing();
				DrawCenteredText("Use the arrow keys to move", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 160, 40, WHITE);
				DrawCenteredText("A key is required to unlock the exit", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 120, 30, GOLD);
				DrawCenteredText("Completing a level increases your timer", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 90, 30, GOLD);
				DrawCenteredText("Try to complete every level before time runs out", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 60, 30, GOLD);
				DrawCenteredText("Press [R] to restart a level", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 30, 30, GOLD);
				DrawCenteredText("Have fun :)", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 30, GOLD);
				DrawCenteredText("Return to menu [ ENTER ]", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 50, 40, WHITE);
			EndDrawing();
			if(IsKeyPressed(KEY_ENTER))
			{
				PlayGameSound("select");
				gameState = 0;
			}
		}
		else if(gameState == 4)
		{
			BeginDrawing();
				DrawTextWave("YOU WON", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 2 - 180, 50, 65, 1, &wcfg2, now, Rainbow());
				DrawCenteredText(TextFormat("Time left: %is", (int)floor(game_timer)), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 120, 30, Rainbow());
				DrawCenteredText("Return to menu [ ENTER ]", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 20, 40, WHITE);
				DrawRectangle(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, 2, WHITE);
			EndDrawing();
			if(IsKeyPressed(KEY_ENTER))
			{
				screenFade = 10;
				ResetGame();
				PlayGameSound("select");
				gameState = 0;
			}
		}
		else if(gameState == 5)
		{
			BeginDrawing();
				DrawCenteredText("You ran out of time", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 150, 40, Rainbow());
				DrawCenteredText(TextFormat("And you finished %i level%c", level + 1, level > 0 ? 's':' '), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 95, 30, Rainbow());
				DrawCenteredText("Return to menu [ ENTER ]", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 20, 40, WHITE);
				DrawRectangle(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, 2, WHITE);
			EndDrawing();
			if(IsKeyPressed(KEY_ENTER))
			{
				screenFade = 10;
				ResetGame();
				PlayGameSound("select");
				gameState = 0;
			}
		}
	}
	CloseAudioDevice();
	CloseWindow();
	return 0;
}