#include "sd_functions.h"
#include "display_functions.h"
#include "dsp_functions.h"
#include "capture_voice_functions.h"
#include "utils.h"
/*================[Private defines]========================*/
#define CODE_VERSION 1
#define NO_DATABASE FR_NO_FILE
#define NO_REGISTER FR_DISK_ERR
/*================[Private functions]======================*/

FRESULT initialize_files(void){
	indicatorMessage message;
	FRESULT res;

	//Chequeo que exista el archivo con la database de usuarios
	res = check_if_file_exists("usuarios.txt");
	if(res != FR_OK){
		return NO_DATABASE;
	}

	//Chequeo si existe el archivo de registro de accesos. Si no existe, lo creo
	res = check_if_file_exists("registro.txt");
	if(res != FR_OK){
		res = create_file("registro.txt", "Fecha Usuario Estado\n");
		if(res != FR_OK){
			return NO_REGISTER;
		}
	}
	return FR_OK;
}

void build_entry_message(char *entry, char *user_name, const char *status){
	get_time_from_rtc(entry);						//Obtengo fecha y hora
	if(user_name != NULL){
		strcat(entry, user_name);					//Concateno nombre de usuario
	}
	else{
		strcat(entry, "Desconocido");				//Si no existe, concateno desconocido
	}
	strcat(entry, " ");
	strcat(entry, status);							//Concateno estado de acceso
	strcat(entry, "\n");
	return;
}

/*================[Public task]==========================*/
#if CODE_VERSION == 1
void sd_task(void *pvParameters){
	//Variables usadas por la tarea SD
	indicatorMessage message;										//Para enviar mensajes al display
	FRESULT res;													//Para guardar los resultados de las operaciones de manejo de archivos
	char user_key_retrieved[SEQUENCE_LENGTH + 1];					//Para recibir los digitos desde el keypad
	char *template_path;											//Para guardar la direccion del template asociado al usuario
	char *user_name;												//Para guardar el nombre de usuario
	char *entry;													//Para guardar la entrada a escribir en el registro
	uint16_t *voice_buf;											//Para guardar la voz capturada (buffer)
	bool is_recognized;												//Para guardar estado de reconocimiento de voz

	//Control inicial. Verifico existencia de archivos importantes
	res = mount_sd("");
	if(res == FR_OK){
		res = initialize_files();									//Compruebo archivos importantes
		if(res != FR_OK){
			if(res == NO_DATABASE){
				message = PANTALLA_DATABASE_NO_EXISTE;
			}
			else if(res == NO_REGISTER){
				message = PANTALLA_REGISTRO_NO_CREADO;
			}
			else{
				message = PANTALLA_ERROR_DESCONOCIDO;
			}
			xQueueSend(display_queue, &message, portMAX_DELAY);		//Envio pantalla al display
			while(1);												//Me quedo en este loop eternamente. Reiniciar y solucionar el problema
		}
		unmount_sd("");												//Desmonto tarjeta SD
	}
	else{
		message = PANTALLA_ERROR_MONTAJE_SD;
		xQueueSend(display_queue, &message, portMAX_DELAY);			//Envio pantalla al display
		while(1);
	}

	while(1){
		//Espero a que llegue la clave de usuario ingresada desde la tarea keypad
		for(uint8_t i = 0; i < SEQUENCE_LENGTH + 1; i++){
			xQueueReceive(sequence_queue, &user_key_retrieved[i], portMAX_DELAY);
		}

		res = mount_sd("");
		if(res == FR_OK){
			//Reservo memoria para almacenar usuario y entrada de registro
			user_name = pvPortMalloc(USER_STR_SIZE * sizeof(char));
			entry = pvPortMalloc(ENTRY_STR_SIZE * sizeof(char));

			//Verifico que la clave exista dentro de la database
			res = search_user("usuarios.txt", user_key_retrieved, user_name);
			if(res == FR_OK){
				//Se encontro la clave
				xSemaphoreGive(sd_display_sync);										//Cedo el semaforo para que lo tome el display
				message = PANTALLA_USUARIO_ENCONTRADO;
				xQueueSend(display_queue, &message, portMAX_DELAY);						//Envio mensaje al display

				//Limpio el carriage return que suele colocar windows
				clear_char(user_name, '\r');

				//Verifico que exista el template
				template_path = pvPortMalloc(TEMPLATE_STR_SIZE * sizeof(char));
				clear_buffer(template_path, TEMPLATE_STR_SIZE);
				snprintf(template_path, TEMPLATE_STR_SIZE, "%s.bin", user_name);		//Compongo nombre del archivo .bin
				res = check_if_file_exists(template_path);
				if(res == FR_OK){
					//Se encontro el template en la SD
					message = PANTALLA_RECONOCIMIENTO_DE_VOZ;
					xQueueSend(display_queue, &message, portMAX_DELAY);
					xSemaphoreTake(sd_display_sync, portMAX_DELAY);						//Espero que el display ceda el semaforo para sincronizar tareas
																						//Inicia reconocimiento de voz luego del countdown

					//Capturo voz del usuario
					voice_buf = pvPortMalloc(VOICE_BUFFER_SIZE * sizeof(uint16_t));
					capture_voice(voice_buf, VOICE_BUFFER_SIZE);
					store_voice(voice_buf, VOICE_BUFFER_SIZE, "current.bin");			//Almaceno voz en la tarjeta SD
					vPortFree(voice_buf);												//Libero memoria utilizada

					//Extraigo features de la voz y lo guardo en la tarjeta SD
					extract_and_save_features("current.bin", "current_feature.bin");

					//Comparo features extraidos con el template bin a bin
					is_recognized = check_voice(template_path, "current_feature.bin");
					if(is_recognized){
						//Se reconocio la voz
						build_entry_message(entry, user_name, "Concedido");
						write_entry("registro.txt", entry);
						message = PANTALLA_VOZ_RECONOCIDA;

						//Enviar acceso a la tarea de la cerradura (pendiente)

					}
					else{
						//No se reconocio la voz
						build_entry_message(entry, user_name, "Denegado");
						write_entry("registro.txt", entry);
						message = PANTALLA_VOZ_NO_RECONOCIDA;
					}
				}
				else{
					//No se encontro el template en la SD
					message = PANTALLA_TEMPLATE_NO_EXISTE;
				}
			}
			else{
				//No se encontro la clave
				build_entry_message(entry, NULL, "Denegado");
				write_entry("registro.txt", entry);
				message = PANTALLA_USUARIO_NO_EXISTE;
			}
		}
		else{
			//No se pudo montar la SD
			message = PANTALLA_ERROR_MONTAJE_SD;
		}

		//Envio la pantalla resultante del procesamiento de datos
		xQueueSend(display_queue, &message, portMAX_DELAY);
	}


}

#elif CODE_VERSION == 2
//Generador de template
void sd_task(void *pvParameters){
	indicatorMessage current_message;
	uint16_t *voice_buffer;
	char filename[NUM_OF_SAMPLES][15] = {"voice_1.bin", "voice_2.bin", "voice_3.bin", "voice_4.bin", "voice_5.bin"};
	//Monto la tarjeta SD. Si  esta correcto, se continua con el procesamiento.
	if(mount_sd("") == FR_OK){
		voice_buffer = pvPortMalloc(VOICE_BUFFER_SIZE * sizeof(uint16_t));
		for(uint8_t i = 0; i < NUM_OF_SAMPLES; i++){
			//Indico al display que se inicia el reconocimiento de voz
			xSemaphoreGive(sd_display_sync);												//Doy el semaforo para que el display pueda tomarlo
			current_message = PANTALLA_RECONOCIMIENTO_DE_VOZ;
			xQueueSend(display_queue, &current_message, portMAX_DELAY);						//Envio el evento de reconocimiento al display
			xSemaphoreTake(sd_display_sync, portMAX_DELAY);									//Bloqueo la tarea hasta que el display me devuelva el semaforo

			//Capturo 1.5 segundos de voz
			capture_voice(voice_buffer, VOICE_BUFFER_SIZE);

			//Envio mensaje al display que indique que se estan procesando los datos
			current_message = PANTALLA_PROCESANDO_DATOS;
			xQueueSend(sd_display_sync, &current_message, portMAX_DELAY);					//En este caso puedo seguir procesando mientras se muestra el mensaje

			//Guardo la voz en la SD
			store_voice(voice_buffer, VOICE_BUFFER_SIZE, filename[i]);						//Guardo voz en el archivo voice_{i+1}.bin
		}
		vPortFree(voice_buffer);

		//Genero template
		generate_template();

		//Envio mensaje al display para indicarle que ya se guardo el template
		xSemaphoreGive(sd_display_sync);
		current_message = PANTALLA_TEMPLATE_GUARDADO;
		xQueueSend(display_queue, &current_message, portMAX_DELAY);
		xSemaphoreTake(sd_display_sync, portMAX_DELAY);										//Bloqueo la tarea hasta que el display me devuelva el semaforo

		//Desmonto tarjeta SD y libero memoria ocupada con filename
		unmount_sd("");
	}

	while(1){

	}
}
#endif


