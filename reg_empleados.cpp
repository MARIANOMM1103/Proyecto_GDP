#include <iostream>
#include <string>
#include <limits>
#include <mysql/mysql.h>
using namespace std;

// Estructura para empelados
struct Empleado {
    string nombreCompleto;
    int id;
    string puesto, curp, direcccion, empresaAsignada;
    string tipoContrato, numeroCredencial, estado, fechaIngreso;
};

//Funcion para conectar a la base de datos 
MYSQL* conectar() {

    MYSQL* conn = mysql_init(0);
    conn = mysql_real_connect(conn, "Aqui van los datos que nos tiene que pasar kevin", 3306, NULL, 0);

    if (conn) {

        cout << "Conexion exitosa";

    }
    else {
        cerr << "Eror al conectar" << mysql_error(conn) << endl;

    }
    return conn;
}


//Funcion para el registro de empleados en la base de datos
void registrarEmpleado(MYSQL* conn) {

    if (!conn) {

        cerr << "No hay conexion con la base de datos";
        return;
    }


    Empleado e;
    cout << "\n--- Registrar Nuevo Empleado ---\n";
    cout << "ID (numero): "; cin >> e.id;
    cin.ignore();
    cout << "Nombre Completo: "; getline(cin, e.nombreCompleto);
    cout << "Puesto: "; getline(cin, e.puesto);
    cout << "CURP: "; getline(cin, e.curp);
    cout << "Direccion: "; getline(cin, e.direccion);
    cout << "Empresa Asignada: "; getline(cin, e.empresaAsignada);
    cout << "Tipo de Contrato (40/48 horas): "; getline(cin, e.tipoContrato);
    cout << "Numero de Credencial (opcional): "; getline(cin, e.numeroCredencial);
    cout << "Estado (Prueba/Completo): "; getline(cin, e.estado);
    cout << "Fecha de Ingreso (YYYY-MM-DD): "; getline(cin, e.fechaIngreso);

    //Crear la consulta sql
    string query = "INSERT INTO empleados (id, nombreCompleto, puesto, curp, direccion, empresaAsignada, tipoContrato, numeroCredencial, estado, fechaIngreso) VALUES (" +
        to_string(e.id) + ", '" + e.nombreCompleto + "', '" + e.puesto + "', '" + e.curp + "', '" + e.direccion + "', '" +
        e.empresaAsignada + "', '" + e.tipoContrato + "', '" + e.numeroCredencial + "', '" + e.estado + "', '" + e.fechaIngreso + "')";   //Esto puede cambiar dependiendo de los nombres de los campos en la tabla de sql
     

        //Ejecutar la consulta 
        if (mysql_query(conn, query.c_str()) == 0) {
            cout << "Empleado registrado correctamente.\n";
        }
        else {
            cerr << "Error al registrar empleado: " << mysql_error(conn) << endl;
        }
}


    int main()
    {
        MYSQL* conn = conectar();
        if (conn) {

            cout << "La conexion fue exitosa";
            mysql_close(conn);
        }
        return 0;
    }


