
Ext.onReady(function(){
    function renderIcon_country(val) 
    { 
        return '<a <img src="./flags/' + val + '.png" ></a>';
    }
    
    function renderIcon_symbol(val) 
    {
        return '<a <img src="./graphics/' + val + '.png" ></a>';
    }

    var store_countries = new Ext.data.Store(
    {
	    proxy: new Ext.data.HttpProxy(
        {
            url: 'list_socks.php',
        }),
	    reader: new Ext.data.JsonReader(
        {
            totalProperty: 'total',
		    root:'results'
        },
        [{name: '0'}, {name: 'ID'},{name: '1'},{name: 'UID'},{name: '2'},{name: 'countrycode'},{name: '3'},{name: 'IP'},{name: '4'},{name: 'lasttime'}])
    });	
    
    var lstCountries = new Ext.grid.GridPanel(
    {
        store: store_countries,
        columns: [
            {
                width: 24,
                dataIndex: 'countrycode',
                renderer: renderIcon_country              
            },
            {
                header   : 'IP', 
                width    : 120, 
                sortable : true,
                dataIndex: 'IP'
            }],
        stripeRows: true
    });
            
    var pan_countries = new Ext.Panel(
    {
        id:'images-view',
        collapsible:false,
        layout:'fit',
        title:'SOCKS Proxies',
        tbar: new Ext.Toolbar({
            enableOverflow: true,
            items: [{
                text: 'Refresh',
                iconCls: 'refreshicon',
                handler:function()
                {
                    store_countries.load();
                }
            }]
        }),
        items: lstCountries
    });
    
	
    var win_countries = new Ext.Window(
    {
        layout:'fit',
        width:240,
        height:300,
        x:850,
        y:115,
        closable: false,
        resizable: true,
        plain: true,
        border: false,
        items: [pan_countries]
	});
    
    store_countries.load();
    
	win_countries.show();
    
});