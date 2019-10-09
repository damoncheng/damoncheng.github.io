<template>

    <div> 
      
      <span>Hello Quasar</span>
      <span class="el-iconfont-chenggong1" style="color:#6BC839"></span>
      <span class="el-icon-success"></span>

    </div>

</template>

<script>

/*
import sqlite3 from 'sqlite3';

var db = new sqlite3.Database(':memory:');

db.serialize(function() {
  db.run("CREATE TABLE lorem (info TEXT)");

  var stmt = db.prepare("INSERT INTO lorem VALUES (?)");
  for (var i = 0; i < 10; i++) {
      stmt.run("Ipsum " + i);
  }
  stmt.finalize();

  db.each("SELECT rowid AS id, info FROM lorem", function(err, row) {
      console.log(row.id + ": " + row.info);
  });
});

db.close();
*/

export default {
  name: 'HomeLayout',
  data () {
    return {
    }
  },
  methods: {

  }
}
</script>

<style>
</style>