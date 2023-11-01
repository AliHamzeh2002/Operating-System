#include "utils.h"

MsgData parse_msg(char* msg){
    MsgData msg_data;
    char* token = strtok(msg, DELIMITERS);
    msg_data.title = token;
    size_t num_of_arguments = 0;
    size_t arguments_size = 2;
    msg_data.arguments = (Argument*)malloc(sizeof(Argument) * arguments_size);
    assert(msg_data.arguments != NULL);
    token = strtok(NULL, DELIMITERS);
    while (token != NULL){
        if (num_of_arguments == arguments_size){
            arguments_size *= 2;
            msg_data.arguments = (Argument*)realloc(msg_data.arguments, sizeof(Argument) * arguments_size);
            assert(msg_data.arguments != NULL);
        }
        msg_data.arguments[num_of_arguments].name = token;
        token = strtok(NULL, DELIMITERS);
        msg_data.arguments[num_of_arguments].value = token;
        num_of_arguments++;
        token = strtok(NULL, DELIMITERS);
    }
    msg_data.num_of_arguments = num_of_arguments;
    return msg_data;
}

char* find_argument(MsgData msg_data, char* argument_name){
    for (int i = 0; i < msg_data.num_of_arguments; i++){
        if (strcmp(msg_data.arguments[i].name, argument_name) == 0)
            return msg_data.arguments[i].value;
    }
    return NULL;
}

char** parse_cmd(char* cmd){
    char** cmd_args = (char**)malloc(sizeof(char*) * MAX_CMD_ARGS);
    assert(cmd_args != NULL);
    char* token = strtok(cmd, DELIMITERS);
    int i = 0;
    while (token != NULL){
        cmd_args[i] = token;
        token = strtok(NULL, DELIMITERS);
        i++;
    }
    cmd_args[i] = NULL;
    return cmd_args;
}



char* json_to_string(const char* json_file_name) {
    int fd = open(json_file_name, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open the file");
        return NULL;
    }
    char* buffer = (char*)malloc(READ_SIZE);
    assert(buffer != NULL);
    ssize_t bytes_read;
    ssize_t total_bytes_read = 0;
    ssize_t buffer_size = READ_SIZE;
    while ((bytes_read = read(fd, buffer + total_bytes_read, READ_SIZE)) > 0) {
        total_bytes_read += bytes_read;
        if (total_bytes_read >= buffer_size) {
            buffer_size *= 2;
            buffer = (char*)realloc(buffer, buffer_size);
            assert(buffer != NULL);
        }
    }
    if (bytes_read == -1) {
        perror("Failed to read the file");
        free(buffer);
        close(fd);
        return NULL;
    }
    buffer[total_bytes_read] = '\0';
    close(fd);
    return buffer;
}

Food* parse_food(cJSON* food_json){
    Food* food = (Food*)malloc(sizeof(Food));
    assert(food != NULL);
    food->name = (char*)malloc(sizeof(char) * strlen(food_json->string));
    assert(food->name != NULL);
    strcpy(food->name, food_json->string);
    food->num_of_ingredients = cJSON_GetArraySize(food_json);
    food->ingredients = (Ingredient*)malloc(sizeof(Ingredient) * food->num_of_ingredients);
    assert(food->ingredients != NULL);
    cJSON* ingredient;
    int i = 0;
    cJSON_ArrayForEach(ingredient, food_json){
        food->ingredients[i].name = (char*)malloc(sizeof(char) * strlen(ingredient->string));
        assert(food->ingredients[i].name != NULL);
        strcpy(food->ingredients[i].name, ingredient->string);
        food->ingredients[i].quantity = ingredient->valueint;
        i++;
    }
    return food;
}

Menu* parse_menu(cJSON* root){
    Menu* menu = (Menu*)malloc(sizeof(Menu));
    assert(menu != NULL);
    menu->num_of_foods = cJSON_GetArraySize(root);
    menu->foods = (Food**)malloc(sizeof(Food*) * menu->num_of_foods);
    assert(menu->foods != NULL);
    cJSON* food_json;
    int i = 0;
    cJSON_ArrayForEach(food_json, root){
        menu->foods[i] = parse_food(food_json);
        i++;
    }

    return menu;
}

Menu* read_json(const char* json_file_name){
    const char* str = json_to_string(json_file_name);
    cJSON *root = cJSON_Parse(str);
    Menu* menu = parse_menu(root);
    cJSON_Delete(root);
    return menu;
}