#include "sd_functions.h"
#include "display_functions.h"
#include "dsp_functions.h"
#include "capture_voice_functions.h"
#include "utils.h"
/*================[Private defines]========================*/
#define CODE_VERSION 1
/*================[Private functions]======================*/
FRESULT initialize_files(void){
	indicatorMessage message;
	FRESULT res;
	if(check_if_file_exists("usuarios.txt" == FR_NO_FILE)){
		message = PANTALLA_DATABASE_NO_EXISTE;
		xQueueSend(display_queue, &message, portMAX_DELAY);
		while(1);
	}
	if(check_if_file_exists("registro.txt") == FR_NO_FILE){
		res = create_file("registro.txt", "Fecha Usuario Estado\n");
		if(res != FR_OK){
			//Mostrar en pantalla que no se pudo crear el archivo
			while(1);
		}
	}
	return FR_OK;
}

/*================[Public task]==========================*/

#if CODE_VERSION == 1
/*Comprobacion de voz simple*/
void sd_task(void *pvParameters){
	//Variables y punteros par el manejo de archivos / keypad
	indicatorMessage current_message;
	char user_key_retrieved[SEQUENCE_LENGTH + 1];							//Buffer asociado a la clave obtenida con el keypad
	char *user_name;														//Donde se almacena el usuario extraido de la base de datos
	char *entry;															//Donde se almacena la entrada nueva del registro
	char *template_path;

	//Variables y punteros utilizados en el procesamiento
	uint16_t *voice_buffer;
	float *current_block;
	float *template_block;
	float *output;
	uint8_t is_recognized;
	uint8_t block_counter;

	//Controles iniciales. Verifico archivos importantes.
	mount_sd("");
	initialize_files();
	unmount_sd("");															//Desmonto tarjeta SD en caso de que sea necesario retirarla

	while(1){
		//Espero a que llegue la clave de usuario ingresada desde la tarea keypad
		for(uint8_t i = 0; i < SEQUENCE_LENGTH + 1; i++){
			xQueueReceive(sequence_queue, &user_key_retrieved[i], portMAX_DELAY);
		}
		xSemaphoreTake(keypad_sd_sync, 0);											//Tomo el semaforo para que el keypad no continue su ejecucion

		//Monto la SD para su uso y reservo memoria para guardar strings
		mount_sd("");
		user_name = pvPortMalloc(USER_STR_SIZE * sizeof(char));						//Reservo espacio en heap para guardar el usuario
		entry = pvPortMalloc(ENTRY_STR_SIZE * sizeof(char));						//Reservo para guardar la entrada del registro

		//Verifico existencia de clave en el registro
		if(search_user("usuarios.txt", user_key_retrieved, user_name) == FR_OK){
			//Si existe el usuario en la base de datos, lo muestro por pantalla
			current_message = PANTALLA_USUARIO_ENCONTRADO;

			//Sincronizo tarea de display y memoria SD
			xSemaphoreGive(sd_display_sync);

			//Envio pantalla al display
			xQueueSend(display_queue, &current_message, portMAX_DELAY);				//Muestro en display que se encontro el usuario

			//Bloqueo la tarea hasta que el display muestre su mensaje
			xSemaphoreTake(sd_display_sync, portMAX_DELAY);

			//Limpio el carriage return
			clear_char(user_name, '\r');

			//Verifico existencia de template
			template_path = pvPortMalloc(TEMPLATE_STR_SIZE * sizeof(char));
			snprintf(template_path, TEMPLATE_STR_SIZE, "%s.bin", user_name);
			if(check_if_file_exists(template_path) == FR_OK){
				//Si existe el template, realizo reconocimiento de voz
				current_message = PANTALLA_RECONOCIMIENTO_DE_VOZ;

				//Sincronizo tarea de display y memoria SD
				xSemaphoreGive(sd_display_sync);

				//Envio pantalla al display
				xQueueSend(display_queue, &current_message, portMAX_DELAY);						//Envio el evento de reconocimiento al display

				//Bloqueo la tarea hasta que el display termine el conteo
				xSemaphoreTake(sd_display_sync, portMAX_DELAY);									//Bloqueo la tarea hasta que el display me devuelva el semaforo
																								//despues del countdown.

				//Capturo 1.5 segundos de voz
				voice_buffer = pvPortMalloc(VOICE_BUFFER_SIZE * sizeof(uint16_t));
				current_block = pvPortMalloc(BLOCK_SIZE * sizeof(float));
				capture_voice(voice_buffer, VOICE_BUFFER_SIZE);
				while(!conv_cplt_flag);															//Espero a que termine la captura
				conv_cplt_flag = false;

				//Mandar cartel procesando valores
				current_message = PANTALLA_PROCESANDO_DATOS;
				xQueueSend(display_queue, &current_message, portMAX_DELAY);

				//Convierto los valores a tension y los guardo en la memoria SD
				for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
					//La conversion es por bloques para ahorrar espacio
					get_voltage(&voice_buffer[i * BLOCK_SIZE], current_block, BLOCK_SIZE);
					save_buffer_on_sd("current voice.bin", current_block, BLOCK_SIZE);
				}

				//Libero memoria utilizada para poder seguir procesando
				vPortFree(voice_buffer);
				vPortFree(current_block);

				//Voy leyendo el template y el current voice
				current_block = pvPortMalloc(BLOCK_SIZE * sizeof(float));
				template_block = pvPortMalloc(BLOCK_SIZE / 2 * sizeof(float)); 					//El modulo de la fft tiene la mitad de tamaño
				output = pvPortMalloc(BLOCK_SIZE / 2 * sizeof(float));
				block_counter = 0;																//Reinicio contador de bloques aceptados
				for(uint8_t i = 0; i < NUM_OF_BLOCKS; i++){
					//Leo template y voz capturada
					read_buffer_from_sd("current voice.bin", current_block, BLOCK_SIZE, i * BLOCK_SIZE);
					read_buffer_from_sd(template_path, template_block, BLOCK_SIZE / 2, i * (BLOCK_SIZE / 2));

					//Proceso la voz capturada
					process_signal(current_block, output, BLOCK_SIZE);

					//Comparo features
					is_recognized = compare_features(output, template_block, BLOCK_SIZE / 2);
					if(!is_recognized){
						break;																	//Salgo del ciclo si algun bloque no fue reconocido
					}
					block_counter++;
				}

				//Obtengo la hora del RTC y empiezo a armar la entrada del registro
				get_time_from_rtc(entry);
				strcat(entry, user_name);														//Concateno fecha, hora y usuario

				//Chequeo si todos los bloques pasaron la prueba
				if(block_counter >= NUM_OF_BLOCKS){
					strcat(entry, " Concedido\n");												//Agrego estado concedido al string

					//Si todos los bloques pasaron la prueba, se reconoce la voz y se da acceso
					current_message = PANTALLA_VOZ_RECONOCIDA;
					xQueueSend(display_queue, &current_message, portMAX_DELAY);					//Envio pantalla de voz reconocida al display
					current_message = PANTALLA_ACCESO_CONCEDIDO;								//Envio pantalla de acceso concedido
				}
				else{
					strcat(entry, " Denegado\n");												//Agrego estado denegado al string
					current_message = PANTALLA_VOZ_NO_RECONOCIDA;
				}

				//Escribo el registro con la nueva entrada
				write_entry("registro.txt", entry);

				//Libero memoria usada para el procesamiento
				vPortFree(current_block);
				vPortFree(template_block);
				vPortFree(output);
			}
			else{
				//Si no existe el template, no realizo reconocimiento de voz y mando la pantalla
				current_message = PANTALLA_TEMPLATE_NO_EXISTE;
			}
		}
		else{
			//Si no se encuentra el usuario, guardo intento de entrada en el registro
			get_time_from_rtc(entry);												//Obtengo fecha y hora del rtc
			strcat(entry, "Desconocido Denegado");
			write_entry("registro.txt", entry);										//Escribo entrada en registro
			current_message = PANTALLA_USUARIO_NO_EXISTE;							//Indico en pantalla que no se encontro el usuario
		}

		//Libero memoria utilizada para escribir el registro
		vPortFree(entry);
		vPortFree(user_name);
		vPortFree(template_path);

		//Desmonto la tarjeta SD
		unmount_sd("");

		//Envio el mensaje al display
		xQueueSend(display_queue, &current_message, portMAX_DELAY);

		//Devuelvo el semaforo a la tarea keypad para que pueda continuar su ejecucion
		xSemaphoreGive(keypad_sd_sync);
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


