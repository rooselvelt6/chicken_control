


function calcular_ganancias_x_socio(cantidad=2){
	if(cantidad == 2){
		return calcular_capital_en_bolsa() / 2;
	}
	else{
		return calcular_capital_en_bolsa() / cantidad;
	}
}


herramientas = [
	{"nombre":"tornillos", "cantidad": 20, "precio_unitario": 0.10},
	{"nombre": "madera", "cantidad": 2, "precio_unitario":0},
	{"nombre": "socates", "cantidad": 6, "precio_unitario":0.4},
	{"nombre": "bombillos", "cantidad": 6, "precio_unitario":0.56},
	{"nombre": "corral", "cantidad": 2, "precio_unitario":0},
	{"nombre": "cama", "cantidad": 2, "precio_unitario":0, "tipo":["paja","hojas","concha de café"]},
	{"nombre": "comedero", "cantidad": 2, "precio_unitario":4},
	{"nombre": "medicinas", "cantidad": 4, "precio_unitario":4},
	{"nombre": "cable", "cantidad": 4, "precio_unitario":1},
	{"nombre": "transporte", "cantidad": 4, "precio_unitario":2},
	{"nombre": "mano_obra", "cantidad": 10, "precio_unitario":0},
	{"nombre": "transporte", "cantidad": 4, "precio_unitario":2},
	{"nombre": "saldo_telefono", "cantidad": 0, "precio_unitario":2},
]

function total_invertido_herramientas(){
	var total = 0;
	for (var i = 0; i < herramientas.length; i++) {
		total += herramientas[i].cantidad * herramientas[i].precio_unitario;
	}
	return total;
}


var alimentos = [
	{"nombre":"preinicial 1",
	 "cantidad": 2,
	 "precio_unitario": 37,
	 "marca": "SUPER S",
	 "analisis": {"humedad":12.50, "grasa": 5, "ceniza": 6, "calcio":1.10, "proteina":23.50, "fibra":3, "fosforo":0.70},
	 "ingredientes": ["maiz amarillo","sorgo", "harina de soya", "soya full fat", "harina de pescado",
	 "harina de carne", "harina de plumas", "harina de sangre","harina de visceras", "arroz partido",
	 "carbonato de calcio","fosfato monodicalcio","sal","grasa animal", "grasa vegetal","vitaminas","minerales"],
	 "inicio": 0,
	 "final":26, 
	 "precauciones": "",
	 "contenido": 40,
	 "presentacion": "semigrano",
	 "fecha_fabricacion": new Date("06/02/2022"),
	 "fecha_vencimiento": new Date("31/03/2022"),
	 "planta": {"ubicación":"valencia", "estado":"carabobo", "telefono":"02418131511", "rif":"J-30609951-4"},
     "asesor": "nelly lopez",
	},
	{"nombre":"preinicial 3",
	 "marca": "SUPER S",
	 "cantidad": 3,
	 "precio_unitario": 37,
	 "analisis": {"humedad":12.50, "grasa": 5, "ceniza": 6, "calcio":1.10, "proteina":23.50, "fibra":3, "fosforo":0.70},
	 "ingredientes": ["maiz amarillo","sorgo", "harina de soya", "soya full fat", "harina de pescado",
	 "harina de carne", "harina de plumas", "harina de sangre","harina de visceras", "arroz partido",
	 "carbonato de calcio","fosfato monodicalcio","sal","grasa animal", "grasa vegetal","vitaminas","minerales"],
	 "inicio": 26,
	 "final":45, 
	 "precauciones": "",
	 "contenido": 40,
	 "presentacion": "grano",
	 "fecha_fabricacion": new Date("06/02/2022"),
	 "fecha_vencimiento": new Date("31/03/2022"),
	 "planta": {"ubicación":"valencia", "estado":"carabobo", "telefono":"02418131511", "rif":"J-30609951-4"},
     "asesor": "nelly lopez",
	},
	// Aplicado por error
	{"nombre":"cerdo engorde",
	 "marca": "granpec",
	 "cantidad": 2,
	 "precio_unitario": 21,
	 "analisis": {"humedad":12, "grasa": 4, "ceniza": 5.50, "calcio":0.65, "proteina":14.50, "fibra":5, "fosforo":0.55},
	 "ingredientes": ["maiz amarillo","maiz blanco", "sorgo", "trigo", "harina de soya", "soya full fat", "harina de pescado",
	 "harina de carne", "harina de plumas hidrolizadas", "harina de sangre","harina de visceras", "arroz partido",
	 "carbonato de calcio","fosfato monodicalcio","sal","grasa animal", "grasa vegetal","vitaminas","minerales",
	 "afrecho de maiz","germen de maiz", "harina de maiz", "afrechillo de trigo", "afrecho de trigo", "germen de trigo",
	 "segunda de arroz","tercerilla de arroz", "pulitura de arroz","cascarilla de arroz","harina de soya","harina de girasol",
	 "aceite de maiz","aceite de soya","grasa amarilla","oleinas","sebo","melaza","aminoácidos sinteticos","cabonato de calcio",
	 "fosforo monodicalcico","tricalcico", "aditivos"],
	 "inicio": "nunca",
	 "final": "nunca", 
	 "precauciones": "no se debe aplicar nunca",
	 "contenido": 40,
	 "presentacion": "semigrano",
	 "fecha_fabricacion": "",
	 "fecha_vencimiento": "",
	 "planta": {"ubicación":"barcelona", "estado":"anzoategui", "telefono":"04128613733", "rif":"J-29556655-7"},
     "asesor": "germán hernandez",
	},
	{"nombre":"preinicial 3",
	 "marca": "SUPER S",
	 "cantidad": 1,
	 "precio_unitario": 44,
	 "analisis": {"humedad":12.50, "grasa": 5, "ceniza": 6, "calcio":1.10, "proteina":23.50, "fibra":3, "fosforo":0.70},
	 "ingredientes": ["maiz amarillo","sorgo", "harina de soya", "soya full fat", "harina de pescado",
	 "harina de carne", "harina de plumas", "harina de sangre","harina de visceras", "arroz partido",
	 "carbonato de calcio","fosfato monodicalcio","sal","grasa animal", "grasa vegetal","vitaminas","minerales"],
	 "inicio": 26,
	 "final":45, 
	 "precauciones": "",
	 "contenido": 40,
	 "presentacion": "grano",
	 "fecha_fabricacion": new Date("06/02/2022"),
	 "fecha_vencimiento": new Date("31/03/2022"),
	 "planta": {"ubicación":"valencia", "estado":"carabobo", "telefono":"02418131511", "rif":"J-30609951-4"},
     "asesor": "nelly lopez",
	},
]

// Funciones para el calculo de los alimentos
function cantidad_sacos_consumido(){
	var total = 0;
	for (var i = 0; i < alimentos.length; i++) {	
		total += alimentos[i].cantidad 
	}
	return total;
}

function total_invertido_alimento(){
	var total = 0;
	for (var i = 0; i < alimentos.length; i++) {	
		total += (alimentos[i].cantidad * alimentos[i].precio_unitario);
	}
	return total;
}

function kg_alimento_consumidos(){
	var total = 0;
	for (var i = 0; i < alimentos.length; i++) {	
		total += (alimentos[i].cantidad * alimentos[i].contenido);
	}
	return total;
}

function calcular_inversion_bruta(){
	// Calcular la cantidad bruta invertida en el proyecto 
	return calcular_gastos_animales() + total_invertido_alimento() + total_invertido_herramientas();
}

function mostrar_reporte(){
	// Control de flujo de salida 
	console.log("Precio del USD actual:", mostrar_precio_dolar())
	console.log("***************************")
	console.log("GRANJA DE POLLOS DE ENGORDE")
	console.log("***************************")
	console.log("Personas encargadas:")
	mostrar_encargados()
	console.log("***************************")
	console.log("Inversores:");
	mostrar_inversores()

	console.log("*********************ANIMALES*****************************")
	console.log(calcular_precio_usd_x_kg(), "$ por Kg");
	console.log("Cantidad invertida en animales:", calcular_gastos_animales(),"$")
	console.log("Perdidas por muertes naturales:", calcular_perdidas_x_muertes(),"$")
	console.log("Capital de reposición por muertes:", animales.reposicion,"$")
	console.log("Capital en trabajo:", calcular_capital_trabajo_animal(),"$");
	console.log("Cantidad sacrificados:", cantidad_sacrificada(), "pollos", "=>" ,total_kg_sacrificados(), "kg")
	console.log("Animales en el corral:", cantidad_corral(), "pollos","=>", total_kg_en_corral(), "kg aproximadamente","se deben pesar");
	console.log("Kg Vendidos:", calcular_kg_vendidos(), "Cantidad de pollos:", cantidad_pollos_vendidos())


	console.log("*********************Alimentos********************")
	console.log("Inversión: ", total_invertido_alimento(),"$")
	console.log("Consumo total:",kg_alimento_consumidos(),"kg", cantidad_sacos_consumido(), "sacos")


	console.log("******************Herramientas***************************")
	console.log("Total en herramientas:", total_invertido_herramientas(),"$");


	console.log("*****************Gastos Realizados***********************")
	console.log("Inversión bruta realizada:",  calcular_inversion_bruta());	
	console.log("*****************VENTAS REALIZADAS************************")
	console.log("Capital recuperado por ventas:", calcular_capital_en_bolsa())
	console.log("Capital repartido a cada socio:",  calcular_ganancias_x_socio(cantidad=2))
	// Fin del flujo de salida
}
