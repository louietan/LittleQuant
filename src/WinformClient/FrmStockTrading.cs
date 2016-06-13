using LittleQuant.Core;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LittleQuant.WinformClient
{
    public partial class FrmStockTrading : Form
    {
        private IStockExchange _exchange = ExchangeManager.GetExchange<IStockExchange>();

        public FrmStockTrading()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this._exchange.SubmitOrder(new StockOrder
            {
                InstrumentID = textBox1.Text,
                Qty = (int)numericUpDown2.Value,
                Price = (double)numericUpDown1.Value,
                Side = radioButton1.Checked ? OrderSide.Buy : OrderSide.Sell,
            });
        }

        private void button2_Click(object sender, EventArgs e)
        {
            var order = this._exchange.Account.PendingOrders.FirstOrDefault(_ => _.OrderID == textBox1.Text);
            this._exchange.CancelOrder(order);
        }
    }
}
