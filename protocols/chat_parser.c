#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/cJSON/cJSON.h"
#include "../lib/cJSON/cJSON_Utils.h"

// Functions to parse Minecraft chat strings/objects
// See https://wiki.vg/Chat

struct Format {
	bool is_color;           // Whether it is stored in the "color" property of a chat object, or its own property
	char mc_code;            // The character after § in a formatted string
	unsigned char ansi_code; // The number used to display it in a console, e.g. 32 in \e[32m
	const char *name;        // The JSON string value of the property defined by is_color
};

const struct Format formats[] = {
	{ .is_color = true,  .mc_code = '0', .ansi_code = 30, .name = "black"         },
	{ .is_color = true,  .mc_code = '1', .ansi_code = 34, .name = "dark_blue"     },
	{ .is_color = true,  .mc_code = '2', .ansi_code = 32, .name = "dark_green"    },
	{ .is_color = true,  .mc_code = '3', .ansi_code = 36, .name = "dark_aqua"     },
	{ .is_color = true,  .mc_code = '4', .ansi_code = 31, .name = "dark_red"      },
	{ .is_color = true,  .mc_code = '5', .ansi_code = 35, .name = "dark_purple"   },
	{ .is_color = true,  .mc_code = '6', .ansi_code = 33, .name = "gold"          },
	{ .is_color = true,  .mc_code = '7', .ansi_code = 37, .name = "gray"          },
    { .is_color = true,  .mc_code = '8', .ansi_code = 90, .name = "dark_gray"     },
	{ .is_color = true,  .mc_code = '9', .ansi_code = 94, .name = "blue"          },
	{ .is_color = true,  .mc_code = 'a', .ansi_code = 92, .name = "green"         },
	{ .is_color = true,  .mc_code = 'b', .ansi_code = 96, .name = "aqua"          },
	{ .is_color = true,  .mc_code = 'c', .ansi_code = 91, .name = "red"           },
	{ .is_color = true,  .mc_code = 'd', .ansi_code = 95, .name = "light_purple"  },
	{ .is_color = true,  .mc_code = 'e', .ansi_code = 93, .name = "yellow"        },
	{ .is_color = true,  .mc_code = 'f', .ansi_code = 97, .name = "white"         },
	{ .is_color = false, .mc_code = 'l', .ansi_code = 1,  .name = "bold"          },
	{ .is_color = false, .mc_code = 'm', .ansi_code = 9,  .name = "strikethrough" },
	{ .is_color = false, .mc_code = 'n', .ansi_code = 4,  .name = "underlined"    },
	{ .is_color = false, .mc_code = 'o', .ansi_code = 3,  .name = "italic"        },
	{ .is_color = true,  .mc_code = 'r', .ansi_code = 0,  .name = "reset"         },
};

char *format_to_ansi(struct Format format)
{
	char *result = malloc(6);
	snprintf(result, 6, "\x1b[%dm", format.ansi_code);
	return result;
}

char *mc_code_to_ansi_string(char code)
{
	for (int i = 0; i < sizeof(formats) / sizeof(struct Format); i++) {
		if (formats[i].mc_code == code) return format_to_ansi(formats[i]);
	}
	return malloc(1);
}

char *chat_string_to_ansi_string(char *string)
{
	char *result = malloc((strlen(string) * 2.5) + 1); // Final size should not exceed 2.5x original size
	int offset = 0;
	char *match;
	while (match = strstr(string, "§")) {
		// Copy everything until section sign
		memcpy(result + offset, string, match - string);
		offset += match - string;

		// Copy replacement	
		char *replacement = mc_code_to_ansi_string(match[2]);
		memcpy(result + offset, replacement, strlen(replacement));
		offset += strlen(replacement);
		free(replacement);

		// Skip to after format code
		string = match + 3;
	}
	// Copy remaining
	strcpy(result + offset, string);
	return result;
}

char *chat_object_to_ansi_string(cJSON *chat_object)
{
	// Get "text" property or return empty string
	cJSON *json_text = cJSON_GetObjectItemCaseSensitive(chat_object, "text");
	if (!cJSON_IsString(json_text) || json_text == NULL) return malloc(1);

	// Get color property for later
	cJSON *json_color = cJSON_GetObjectItemCaseSensitive(chat_object, "color");
	char *color = cJSON_IsString(json_color) ? json_color->valuestring : NULL;

	// Create format string
	char *ansi_format = malloc(18);
	strcpy(ansi_format, "\x1b[");
	bool has_formatting = false;
	for (int i = 0; i < sizeof(formats) / sizeof(struct Format); i++) {
		if (
			// If it's the same color as the color property
			(color && formats[i].is_color && strcmp(color, formats[i].name) == 0) || 
			// or if it's special formatting and its own property is set to true
			cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(chat_object, formats[i].name))
		) {
			// Add the ANSI code + a semicolon to the format string
			sprintf(ansi_format, "%s%d;", ansi_format, formats[i].ansi_code);
			has_formatting = true;
		}
	}

	char *text = chat_string_to_ansi_string(json_text->valuestring);

	// Join formatting and text together
	char *result;
	if (has_formatting) {
		ansi_format[strlen(ansi_format) - 1] = 'm';
		result = malloc(strlen(ansi_format) + strlen(text) + 1);
		strcpy(result, ansi_format);
		strcat(result, text);
		free(text);
	} else {
		result = text;
	}

	free(ansi_format);

	// Merge object and call recursively
	cJSON *siblings = cJSON_GetObjectItemCaseSensitive(chat_object, "extra"), *sibling = NULL;
	cJSON_ArrayForEach(sibling, siblings) {
		// Create new object which merges the properties of the current and sibling object
		cJSON *merged_sibling = cJSON_Duplicate(chat_object, false);
		cJSONUtils_MergePatchCaseSensitive(merged_sibling, sibling);

		// Recursively call this function to get sibling result
		char *sibling_text = chat_object_to_ansi_string(merged_sibling);
		//char *sibling_text = cJSON_GetObjectItemCaseSensitive(merged_sibling, "text")->valuestring;
		result = realloc(result, strlen(result) + strlen(sibling_text) + 1);
		strcat(result, sibling_text);

		// Clean up
		cJSON_Delete(merged_sibling);
		free(sibling_text);
	}
	
	return result;
}

