/*  Sistema: "Control de inversión de granja de animales en Caripe"
	Creador: "Rooselvelt Angulo"
	Fecha de inicio: 28/01/2022
*/

console.log("Sistema de control")
*/

// Objeto que gestiona el dolar actual
var dolar = {
	"precio": 4340,
	// Función obtener y actualizar precio
}

function mostrar_precio_dolar(){
	return dolar.precio;
}

function actualizar_precio_dolar(precio_actual){
	dolar.precio = precio_actual;
}


// Objeto Animal
let animales = {
	"lote": 1, // Número del lote
	"produccion": "Carne", // Tipo de producción de la granja: ["huevos", "carne"]
	"tipo": "pollo de engorde", // Tipo de carne
	"raza": "primera", // Raza del pollo
	"cantidad": 70, // Cantidad adquirida
	"precio_unitario": 1, // Precio unitario de cada animal
	"aumento_unitario": 0, // Aumentos realizados por el distribuidor
	"precio_ves": 15000,
	"fecha_inicial": new Date("02/02/2022"), // Descargar biblioteca para el control de fechas
	"fecha_final": new Date("/03/19/2022"), // Descargar biblioteca para el control de fechas
	"periodo_duracion": 45, // Duración del ciclo de crianza
	"fecha_actual" : new Date("04/10/2022"), // Descargar biblioteca para el control de fechas
	"dias_activos": "", // Descargar biblioteca para el control de fechas
	"cantidad_meses": "", // Descargar biblioteca para el control de fechas
	"cantidad_semanas": "", // Descargar biblioteca para el control de fechas
	"muertes": [{"causa": "aplastamiento", "cantidad": 4},
				{"causa": "ahogamiento", "cantidad": 2},
				{"causa": "gases tóxicos", "cantidad": 1},
				{"causa": "infarto", "cantidad": 2},
				{"causa": "retención de líquidos", "cantidad": 2},
			   ],
	"reposicion": 10,
	"sacrificios":  [2.0,2.6,2.2,2.2,2.1,
								1.8,2.4,2.2,2.3,2.5,
								2.6,2.6,2.5,2.4,2.4,
								2.4,2.4,2.3,2.3,2.3, // Animales Sacrificados
								2.4,2.1,2.1,2.1,2.1,
								2.1,2.5,2.3,2.6,2.1,
								2.4,2.3,2.1,2.3,2.1,
								2.2,2.2,2.3,2.3,2.3,
								2.3,2.3],

	"vivos":  [2.0,2.0,2.0,2.0,2.0,
			   2.0,2.0,2.0,2.0,2.0,
			   2.0,2.0,2.0,2.0,2.0, // Animales Vivos actualmente en el corral
			   1.0,1.0,1.0,1.0,1.0,
			   1.0,1.0,1.0,1.0,1.0,],

	"congelados": [],

	"vendidos": [
					{
					"cliente":"alfredo",
				 	"cantidad":1,
				 	"pesos":[2.2], 
				 	"pago": true,
				 	"metodo_pago":["dolares","bolivares"],
				 	"referencia": "felix",
				 	
					 },

					{
						"cliente":"xiomara",
					 	"cantidad":3,
					 	"pesos":[2.2,2.5,2.4],
					 	"pago": true,
					 	"metodo_pago":["dolares","bolivares", "pago móvil"],
					 	"referencia": "nelo",
					 	
					 },

					 {
					 	"cliente":"julio picha",
						"cantidad":1,
						"pesos":[2.3],
						"pago": true,
						"metodo_pago":["pago móvil"],
						"referencia": "felix",
					 },


					 {
					 	"cliente":"danilo",
					 	"cantidad":4,
					 	"pesos":[2.3, 2.1, 2.1, 2.3],
					 	"precios": [34500, 31500, 31500, 34500],
					 	"total": 132000,
					 	"abonado": 75100,
					 	"debe": 56900, // 12.61$
					 	"pago": false,
					 	"metodo_pago":["dolares","bolivares"],
					 	"debe": [2.1,2.3], // Pollo de 2.1
						"referencia": "felix",
					 },


					 {	
					 	"cliente":"cataco",
					 	"cantidad":1,
					 	"pesos":[2.0],
					 	"pago": true,
					 	"metodo_pago":["efectivo"],
					 	"referencia": "felix",
					 },

					 {
					 	"cliente":"frank",
					 	"cantidad":5,
					 	"pesos":[2.6,2.6,2.5,2.4,2.4],
					 	"pago": true,
					 	"metodo_pago":["pago móvil", "dolares"],
					 	"referencia": "felix",
					 	"dirección": "casa de madera",
					 },

					 {
					 	"cliente":"lorenzo cocina",
					 	"cantidad":1,
						"pesos":[2.3],
					 	"pago": false,
					 	"metodo_pago":["pago móvil"],
					 	"referencia": "felix",
					 },

					 {
					 	"cliente":"élibeth",
					 	"cantidad":1,
					 	"pesos":[2.4],
					 	"pago": true,
					 	"metodo_pago":["efectivo"],
					 	"referencia": "felix",
					 },

					 {
					 	"cliente":"kike cueva",
					 	"cantidad":1,
					 	"pesos":[2.4],
					 	"pago": true,
					 	"metodo_pago":["pago móvil"],
					 	"referencia": "felix",
					 },

					 {
					 	"cliente":"casa",
					 	"cantidad":1,
					 	"pesos":[2.1],
					 	"pago": false,
					 	"metodo_pago":["pago móvil"],
					 	"referencia": "felix",
					 },
					
					{
					 	"cliente":"ranielt",
					 	"cantidad":2,
					 	"pesos":[2.3, 1.8],
					 	"pago": false,
					 	"metodo_pago":["pago móvil"],
					 	"referencia": "rooselvelt",
					 },

				],

}



function calcularCapital(){
	/* Calcular la cantidad de kg de pollos vendidos
	let total = 0;
	for (let i = 0; i <= animales.vendidos.length -1; i++) {
		console.log(animales.vendidos[i]["cliente"],animales.vendidos[i]["cantidad"], animales.vendidos[i]["pesos"]); 		
		for(let j=0; j <= animales.vendidos[i]["pesos"].length -1; j++){
			console.log(animales.vendidos[i]["pesos"][j])
			total += animales.vendidos[i]["pesos"][j];

		}
	}
	return total * calcular_precio_pollo_usd();*/
	return calcular_kg_vendidos() * calcular_precio_pollo_usd();
}

// Funciones generales para el control del inventario de animales
function cantidad_pollos_vendidos(){
	var cantidad = 0;
	for (var i = 0; i < animales.vendidos.length; i++) {
		cantidad += (animales.vendidos[i].cantidad);
	}
	return cantidad;
}

function calcular_kg_vendidos(){
	// Calcular la cantidad de kg de pollos vendidos
	var total = 0;
	for (var i = 0; i < animales.vendidos.length; i++) {
		for (var j = 0; j < animales.vendidos[i].pesos.length; j++) {
			total += animales.vendidos[i].pesos[j];
		}	
	}
	return total;
}

function mostrarNumeroLote(){
	return animales.lote;
}

function mostrarTipoProduccion(){
	return animales.produccion
}

function mostrarTipoAnimales(){
	return animales.tipo
}

function mostrarCantidadComprada(){
	return animales.cantidad;
}

function calcularPrecioUnitario(){
	return animales.precio_unitario + animales.aumento_unitario;
}

function incrementarAumentoUnitario(cantidad){
	animales.aumento_unitario =  animales.aumento_unitario + cantidad;
}

function calcularGastoAnimal(){
	return (animales.precio_unitario + animales.aumento_unitario) * animales.cantidad
}

function mostrarAnimalesMuertos(){
	for (var i = 0; i < animales.muertes.length; i++) {
		console.log("(",animales.muertes[i].cantidad, ")","por",animales.muertes[i].causa, ":", );
	}
}

function mostrarCantidadAnimalesMuertos(){
	let total = 0;
	for (var i = 0; i < animales.muertes.length; i++) {
		total += animales.muertes[i].cantidad;
	}
	return total;
}

function calcularPerdidasPorMuertes(){
	return mostrarCantidadAnimalesMuertos() * calcularPrecioUnitario();
}

function reponerAnimales(cantidad){
	animales.reposicion += cantidad;
}

function mostrarCantidadRepuesta(){
	return animales.reposicion;
}

function inversionAnimalActiva(){
	return Number(calcularGastoAnimal() - calcularPerdidasPorMuertes() + mostrarCantidadRepuesta());
}

function total_kg_en_corral(){
	var total = 0;
	for (var i = 0; i < animales.vivos.length; i++) {
		total += animales.vivos[i];
	}
	return total;
}

function cantidad_corral(){
	return animales.vivos.length;
}

function total_kg_sacrificados(){
	var total = 0;
	for (var i = 0; i < animales.sacrificios.length; i++) {
		total += animales.sacrificios[i];
	}
	return total;
}

function cantidad_sacrificada(){
	return animales.sacrificios.length;
}


function obtenerAnimalesCongelados(){
	return cantidad_sacrificada() - cantidad_pollos_vendidos();
}

function calcularKgCongelados(){
	// Sacar la cantidad de pollos vendidos de la lista de sacrificios y crear otra lista nueva
}

function calcular_precio_pollo_usd(){
	return (animales.precio_ves / dolar.precio);
}

// Objeto inversión
let inversion_inicial = {

	"inversores": [{"nombre": "leomarlys bejarano", "cantidad": 210},
					{"nombre": "felix peroza", "cantidad": 1},
				  ],
	// Mostrar lista de inversores con la cantidad invertida
	
	"encargados": ["rooselvelt angulo", "milagros lezama"], 
	// Mostrar lista de encargados
}

function mostrar_encargados(){
	for (let i = 0; i < inversion_inicial.encargados.length; i++) {
		console.log("Encargado:",1+i,inversion_inicial.encargados[i]);
	}
}

function mostrar_inversores(){
	for (let i = 0; i < inversion_inicial.inversores.length; i++) {
		console.log(inversion_inicial.inversores[i].nombre, ":",inversion_inicial.inversores[i].cantidad,"$");
	}
}

function mostrarVentas(){
	
	for (var i = 0; i < animales.vendidos.length; i++) {
		console.log(animales.vendidos[i].cliente);
		for (var i = 0; i < animales.vendidos[i].pesos.length; i++) {
			
		}
	}
}

// Reporte de Salida
console.log("********************************************************************************************************")
console.log("										Sistema de Control 												 ")
console.log("**************************************************")
console.log("Precio del USD actual:", mostrar_precio_dolar(),"Bolivares")

function reporteAnimal(){
	console.log("********************************************************************************************************")
	console.log("										  ANIMALES 														 ")
	console.log("**************************************************")
	console.log("Precio por Kg actual:", calcular_precio_pollo_usd(),"$")
	console.log("**************************************************")
	console.log("Lote N°:",mostrarNumeroLote(), "Tipo de Producción:", mostrarTipoProduccion(), "de", mostrarTipoAnimales() );
	console.log("Cantidad comprada inicialmente:",mostrarCantidadComprada())
	console.log("Precio unitario de C/U:",calcularPrecioUnitario(),"$")
	console.log("Inversión Bruta en Pollos:", calcularGastoAnimal(), "$")
	console.log("Animales Muertos:", mostrarCantidadAnimalesMuertos(), "=> Pérdidas:",calcularPerdidasPorMuertes(),"$")
	console.log(mostrarAnimalesMuertos())
	console.log("Animales para reposición:",mostrarCantidadRepuesta(),"=>", mostrarCantidadRepuesta(),"$");
	console.log("Inversión activa:", inversionAnimalActiva(),"$")
	console.log("Pollos Beneficiados:", cantidad_sacrificada(), "y se obtuvieron:", total_kg_sacrificados(),"Kg" )
	console.log("**********")
	console.log("**VENTAS**")
	console.log("**********")
	console.log("Animales Vendidos:",cantidad_pollos_vendidos(), "con un total de:",calcular_kg_vendidos(),"Kg")
	//mostrarVentas()
	console.log("***********************************************************************************************")
	console.log("En el corral hay un total de:", cantidad_corral(), "con un total de:", total_kg_en_corral(), "Kg")
	console.log("Congelados:", obtenerAnimalesCongelados())
	console.log("Capital Recuperado:", calcularCapital());
	
}



