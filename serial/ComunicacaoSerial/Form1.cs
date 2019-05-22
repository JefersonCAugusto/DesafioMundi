using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.SqlClient;
using MundiAPI.PCL;
using MundiAPI.PCL.Models;

namespace ComunicacaoSerial
{




    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        //inicializa banco de dados
        SqlConnection conn = new SqlConnection("Data Source=(local);Initial Catalog=Prometeus;Integrated Security=True");
        SqlCommand comando = new SqlCommand();
        SqlDataReader dr;       

        //controle de fluxo da porta serial. aberto ou fechado
        private void Button1_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                timer1.Stop();
                textBox2.Text = "Fechado";
            }
            else
            {
                serialPort1.Open();
                timer1.Start();
                textBox2.Text = "Aberto";

                 
            }
        }
        //a rotina é um timer de 100ms para monitorar a porta de comunicação (COM4) ao aguardo de uma TAG RFID.
        //Quando chegar um conjunto de 40bits for redebido nessa porta, tal informação será armazenada na variavel X1

        private void Timer1_Tick(object sender, EventArgs e)
        {
         

            String x1 = "";

         

            if (serialPort1.IsOpen)
            {
                
                comando.Connection = conn;      // parametro para banco de dados

                 x1 = serialPort1.ReadExisting();      
                
            }

            //A rotina abaixo verifica se a variavel x1 recebeu a instrução da tag de 40bits atravez da porta COM4.
            //caso X1 ainda tenha o valor null continuará presa em um lopp aguardando TAG e essa tag será verificada novamente a cada
            //100ms devido ao timer implementado em "void Timer1_Tick"
            if (x1!= "")
                {
                
             //Assim que for feita a leitura de uma TAG RFID na porta serial emulada a variavel X1 assume o valor dessa TAG, afim de que seja
             //realizado um select no bando de dados em busda id do cliente relacionado.

                    conn.Open();
                    comando.CommandText = "select id from pessoa where RFID = '" + x1 + "'";
                    comando.ExecuteScalar();
                    SqlDataReader dr = comando.ExecuteReader();
                    dr.Read();
                    var idcliente = Convert.ToString(dr["id"]);
                    conn.Close();
           
                    x1 ="";
                    textBox1.Text = "";
                    textBox1.AppendText(idcliente);

                //A rotina abaixo realiza a consulta na base de dados da mundipagg, atravez do Id do cliente(carregada na variavel cachorrinho), 
                //retornando os dados de nome, email,...
                string basicAuthUserName = "sk_test_4AdjlqpseatnmgbW";
                string basicAuthPassword = "";
                var client = new MundiAPIClient(basicAuthUserName, basicAuthPassword);
                var response1 = client.Customers.GetCustomer(idcliente);
                textBox3.Text = response1.Name;
                textBox4.Text = response1.Email;
                textBox5.Text = response1.Type;


                conn.Open();
                comando.CommandText = "UPDATE pessoa SET email = '"+response1.Email+"', name = '"+response1.Name+"'   WHERE id  = '" + idcliente+ "'";
                
                comando.ExecuteNonQuery();
                conn.Close();

                //a etapa abaixo sera de requeisitar dados do cartão passando o id do cliente.


                //aqui seria realizado a busca dos dados do cartão do cliente, mas nao consegui informação de como trabalhar com os dados
                //que receber dentro da String response2
                // var response2 = client.Customers.GetCards(idcliente);


                if (checkBox1.Checked)
                {
                 

                  var valor = textBox8.Text;
                  var cart = textBox9.Text;
                  var request = new CreateChargeRequest()



                        {
                           
                            Amount = Convert.ToInt32(valor),
                          

                            CustomerId = idcliente,

                            Payment = new CreatePaymentRequest()
                            {

                                PaymentMethod = "credit_card",

                                CreditCard = new CreateCreditCardPaymentRequest()
                                {

                                    //colocar proteção de if
                                    CardId = cart,
                                    Card = new CreateCardRequest
                                    {
                                        Cvv = "353",
                                    }
                                }
                            }
                        };

                   
                    {
                        try {
                                var response = client.Charges.CreateCharge(request);
                                textBox6.Text = response.Status;
                                textBox5.Text = response.Id;
                                checkBox1.Checked = false;
                            }
                        catch
                        {
                            MessageBox.Show("Esse Cartão não pertence ao cliente "+ textBox3.Text +". Por favor altere os dados");
                            textBox9.Focus();
                        }


                    }


                    }
















            }
        }

        private void Button2_Click(object sender, EventArgs e)
        {
            SqlConnection conn = new SqlConnection("Data Source=(local);Initial Catalog=Prometeus;Integrated Security=True");
            SqlCommand comando = new SqlCommand();
            comando.Connection = conn;
           
            conn.Open();
            comando.CommandText = " DELETE FROM pessoa";
            comando.ExecuteNonQuery();
            comando.CommandText = "insert into pessoa values( 'F3CF68C793','cus_LDz5gdBho0SKrlA1',null, null)";
            comando.ExecuteNonQuery();
            comando.CommandText = " insert into pessoa values( '16734BB59B','cus_48rgjZmsOYizlgL3',null, null)";
            comando.ExecuteNonQuery();
            comando.CommandText = "  insert into pessoa values( '4E68CA6985','cus_LQXAvdIOrfXBqje5',null, null)";
            comando.ExecuteNonQuery();

            conn.Close();
            MessageBox.Show("Os dados do banco foram apagados");
        }
    }
}
