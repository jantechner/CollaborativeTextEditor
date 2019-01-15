using System;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace CollaborativeTextEditorClient
{
    public class Connection
    {
        private readonly Connection conn = null;
        private Form1 form1 = null;
        private Form2 form2 = null;
        private Form3 form3 = null;
        private Socket socket = null;
        private IPEndPoint endPoint = null;
        private IPAddress address = null;
        private ClientSocketObject state = null;
        private byte[] sendBuffer = null;
        private readonly int port;

        public Connection(Form1 form1, String addr, String port)
        {
            try
            {
                this.conn = this;
                this.form1 = form1;
                this.port = Int32.Parse(port);
                if (IPAddress.TryParse(addr, out IPAddress tempAddress))
                {
                    this.address = tempAddress;
                    form1.SetStatusLabelDelegate("IP address correct!");
                }
                else
                {
                    form1.SetStatusLabelDelegate("Wait! DNS query...");
                    Dns.BeginGetHostEntry(addr, new AsyncCallback(GetHostEntryCallback), null);
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("1. Exception:\t\n" + exc.Message.ToString());
                form1.SetStatusLabelDelegate("Check \"Server Info\" and try again!");
                form1.SetButtonDelegate(true);
            }
        }

        public void Close()
        {
            try
            {
                Send("r");
            }
            catch (Exception exc)
            {
                MessageBox.Show("2. Exception:\t\n" + exc.Message.ToString());
            }
        }

        private void GetHostEntryCallback(IAsyncResult ar)
        {
            try
            {
                IPHostEntry hostEntry = null;
                IPAddress[] addresses = null;

                hostEntry = Dns.EndGetHostEntry(ar);
                addresses = hostEntry.AddressList;
                this.address = addresses[0];

                form1.SetStatusLabelDelegate("Host name correct!");
            }
            catch (Exception exc)
            {
                MessageBox.Show("3. Exception:\t\n" + exc.Message.ToString());
                form1.SetStatusLabelDelegate("Wrong host name!");
                form1.SetButtonDelegate(true);
            }
        }

        public void CreateSocket()
        {
            try
            {
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                endPoint = new IPEndPoint(address, port);
                form1.SetStatusLabelDelegate("Socket successfully created!");
            }
            catch (Exception exc)
            {
                MessageBox.Show("4. Exception:\t\n" + exc.Message.ToString());
                form1.SetStatusLabelDelegate("Check \"Server Info\" and try again!");
                form1.SetButtonDelegate(true);
            }
        }

        public void Connect()
        {
            try
            {
                form1.SetStatusLabelDelegate("Wait! Connecting...");
                socket.BeginConnect(endPoint, new AsyncCallback(ConnectCallback), socket);
                Console.WriteLine("After connect");
            }
            catch (Exception exc)
            {
                MessageBox.Show("5. Exception:\t\n" + exc.Message.ToString());
                form1.SetStatusLabelDelegate("Check \"Server Info\" and try again!");
                form1.SetButtonDelegate(true);
            }
        }

        private void ConnectCallback(IAsyncResult ar)
        {
            try
            {
                state = new ClientSocketObject { fd = (Socket)ar.AsyncState };
                state.fd.EndConnect(ar);

                form1.SetStatusLabelDelegate("Connected to server!");
                while (true)
                {
                    if (state.fd.Poll(-1, SelectMode.SelectRead))
                    {
                        StartReceiving();
                    }
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("6. Exception:\t\n" + exc.Message.ToString());
                form1.SetStatusLabelDelegate("Check \"Server Info\" and try again!");
                form1.SetButtonDelegate(true);
            }
        }

        public void StartReceiving()
        {
            try
            {
                form1.SetStatusLabel("Wait! Reading files...");
                state.fd.BeginReceive(state.buffer, 0, ClientSocketObject.BUF_SIZE, 0, new AsyncCallback(ReceiveCallback), state);
            }
            catch (Exception exc)
            {
                MessageBox.Show("7. Exception:\t\n" + exc.Message.ToString());
                form1.SetStatusLabelDelegate("Check \"Server Info\" and try again!");
                form1.SetButtonDelegate(true);
            }
        }

        public void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                ClientSocketObject state = (ClientSocketObject)ar.AsyncState;
                int size = state.fd.EndReceive(ar);
                state.stringBuilder.Append(Encoding.ASCII.GetString(state.buffer, 0, size));
                //Console.WriteLine(Encoding.ASCII.GetString(state.buffer, 0, size));
                if (!state.stringBuilder.ToString().Contains("<<<!EOF!>>>"))
                {
                    state.fd.BeginReceive(state.buffer, 0, ClientSocketObject.BUF_SIZE, 0, new AsyncCallback(ReceiveCallback), state);
                }
                else
                {
                    form1.SetStatusLabel("Done.");
                    form1.SetButtonDelegate(true);
                    String filesString = state.stringBuilder.ToString();
                    Char action = filesString[0];
                    filesString = filesString.Remove(filesString.IndexOf("<<<!EOF!>>>", 0));  // remove EOF character sequence
                    filesString = filesString.Remove(0, 2); // remove action character

                    if (action == 'f')
                    {
                        form2 = new Form2(filesString, form1, conn);
                        form1.OpenNextFormDelegate(filesString, form2);
                    }
                    else if (action == 'F')
                    {
                        form2.UpdateFilesListDelegate(filesString);
                    }
                    else if (action == 'o')
                    {
                        int index = Int32.Parse(filesString.Substring(0, filesString.IndexOf('#')));
                        filesString = filesString.Remove(0, filesString.IndexOf('#') + 1);
                        form3 = new Form3(filesString, form2, conn, index);
                        form2.OpenNextFormDelegate(filesString, form3);
                    }
                    else if (action == 'u')
                    {
                        filesString = filesString.Remove(0, filesString.IndexOf('#') + 1);
                        form3.UpdateFileContentDelegate(filesString);
                    }
                    state.stringBuilder = state.stringBuilder.Remove(0, state.stringBuilder.Length);
                    Array.Clear(state.buffer, 0, state.buffer.Length);
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("8. Exception:\t\n" + exc.Message.ToString());
                form1.SetStatusLabelDelegate("Check \"Server Info\" and try again!");
                form1.SetButtonDelegate(true);
            }
        }

        public void Send(String message)
        {
            try
            {
                message += "#<<<!!!EOF!!!>>>";
                ClientSendMessage obj = new ClientSendMessage { message = message, socket = socket };
                Console.WriteLine(obj.message);
                sendBuffer = Encoding.ASCII.GetBytes(message);
                socket.BeginSend(sendBuffer, 0, sendBuffer.Length, 0, new AsyncCallback(SendCallback), obj);
            }
            catch (Exception)
            {
                //MessageBox.Show("9. Exception:\t\n" + exc.Message.ToString());

            }
        }

        private void SendCallback(IAsyncResult ar)
        {
            try
            {
                ClientSendMessage obj = (ClientSendMessage)ar.AsyncState;
                int bytesSent = obj.socket.EndSend(ar);
                Console.WriteLine("Sent " + bytesSent + " bytes to the client");
                if (obj.message == "r")
                {
                    try
                    {
                        socket.Shutdown(SocketShutdown.Both);
                        socket.Close();
                    }
                    catch (Exception) { }
                }
            }
            catch (Exception exc)
            {
                MessageBox.Show("10. Exception:\t\n" + exc.Message.ToString());
            }
        }
    }

    public class ClientSocketObject
    {
        public const int BUF_SIZE = 1024;
        public byte[] buffer = new byte[BUF_SIZE];
        public StringBuilder stringBuilder = new StringBuilder();
        public Socket fd = null;
    }

    public class ClientSendMessage
    {
        public Socket socket = null;
        public String message = null;
    }
}
