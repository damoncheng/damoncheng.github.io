<template>

    <div> 
      
        <!-- 主要包裹组件 -->
        <q-layout>

          <!-- 可选 -->
          <q-layout-header>
            <!-- 任何内容 -->

            <q-toolbar>
              <!-- showLeft是附加到下面的左侧侧滑面板的模型-->
              <q-btn
                flat round dense
                @click="showLeft = !showLeft"
                icon="menu"
                v-show="direction ==='left'"
              />

              <q-btn
                flat round dense
                @click="goBack()"
                class="icon iconfont el-iconfont-fanhui"
                v-show="direction ==='right'"
              />

              <!-- 当direction为right的时候，显示回退按钮-->

              <q-toolbar-title>
                damoncheng笔记库
                <!-- <span slot="subtitle">Optional subtitle</span>-->
              </q-toolbar-title>

              <!-- showRight是附加到下面的右侧侧滑面板的模型 -->
              <q-btn
                flat round dense
                @click="showRight = !showRight"
                icon="menu"
                v-show="direction ==='right'"
              />
            </q-toolbar>

          </q-layout-header>

          <!-- 可选 -->
          <q-layout-drawer side="left"  v-model="showLeft">
            <!-- 任何内容 -->
              <q-list no-border link inset-separator>
                <q-list-header>知识领域</q-list-header>

                <q-item v-for="(domain,index) in this.domain_url_arr" :key="index" :to="domain.url">
                  <q-item-side icon="school" />
                  <q-item-main :label="domain.name" />
                </q-item>
              
              </q-list>
         </q-layout-drawer>

          <!-- 可选 -->
          <q-layout-drawer side="right"  v-model="showRight">
                <q-list-header>{{domain}}领域</q-list-header>
                <q-item v-for="(module,index) in this.module_url_arr" :key="index" :to="module.url">
                  <q-item-side icon="school" />
                  <q-item-main :label="module.name" />
                </q-item>
          </q-layout-drawer>

          <!-- 必需的 -->
          <q-page-container>

            <!--
              这里是Vue Router注入
              子页面组件的地方

              下面的<router-view />标签可以替换成一个
              实际的网页内容，你应该这样做。
            -->
            <router-view />
            <!--
              QPageContainer的第一个子无素必须是QPage，
              所以请确保您的布局路由子组件
              封装一个QPage。
            -->

          </q-page-container>

          <!-- 可选  -->
          <q-layout-footer>
            <!-- 任何内容 -->
          </q-layout-footer>
        </q-layout>

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
        showLeft: false,
        showRight: false,
        direction: "left",
        domain: '',
        domain_url_arr: [],
        module_url_arr: [],
        path: '/',
        //section_url_arr: []
    }
  },
  computed: {


  },
  methods: {

    goBack(){

        let path_parts = this.path.split('/')

        let go_back_path = "/"
        if(path_parts.length > 3){

          go_back_path = path_parts.slice(0, -2).join('/')

        }

        this.$router.push(go_back_path)

    }

  },
  watch: {

    '$route' (to, from) {

      // 对路由变化作出响应...
      if(to.params.domain){

          this.domain = to.params.domain
          this.direction = "right"
          this.showLeft = false

      }else{

          this.direction = "left"
          this.showRight = false

      }

      
      if(to.params.domain){

        this.module_url_arr = []

        for(let one_module in this.$store.state.markdown.obj[to.params.domain]){

            this.module_url_arr.push({

              name : one_module,
              url : `/domain/${to.params.domain}/module/${one_module}`

            })

        }
        
      }

      this.path = to.path

      /*
      if(to.params.module){

        this.section_url_arr = []
        for(let one_section of this.$store.state.mardown[to.params.domain][to.params.module]){

          this.section_url_arr.push(one_section)

        }

      }
      */


    }

  },
  created : function() {


    //console.log(markdown)
    //this.markdown = markdown
    for(let domain in this.$store.state.markdown.obj){
        this.domain_url_arr.push({ name : domain, url : `/domain/${domain}`})
      }

    console.log(`this.doamin_url_arr`)
    console.log(this.domain_url_arr)


  }
}
</script>

<style>
</style>